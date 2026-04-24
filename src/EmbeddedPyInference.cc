#include "DDML/EmbeddedPyInference.h"

#include <cassert>
#include <cstdlib> // setenv
#include <cstring>
#include <mutex>
#include <stdexcept>

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace ddml {

struct __attribute__((visibility("hidden"))) EmbeddedPyInference::Impl {
  py::object callable;
  py::list pyInputs;
  std::vector<py::array_t<float>> inputArrays;
  std::vector<const float*> lastDataPtrs;
  std::vector<std::vector<ssize_t>> lastShapes;
};

namespace {
  std::once_flag g_interpInit;
  void ensureInterpreter() {
    // Leaked on purpose: process-lifetime interpreter.
    // Finalising CPython with NumPy loaded is unreliable.
    std::call_once(g_interpInit, [] {
      if (Py_IsInitialized()) {
        // Host (e.g. ddsim running via cppyy) already owns the interpreter;
        // do not create a second one.
        return;
      }
      static py::scoped_interpreter interp{};
      (void)interp;
    });
  }

  std::string formatPyError(py::error_already_set& e) {
    try {
      py::module_ tb = py::module_::import("traceback");
      py::object fmt = tb.attr("format_exception")(e.type(), e.value(), e.trace());
      std::string out;
      for (auto line : fmt) {
        out += line.cast<std::string>();
      }
      return out;
    } catch (...) {
      return std::string{e.what()};
    }
  }
} // namespace

EmbeddedPyInference::EmbeddedPyInference() : m_impl(std::make_unique<Impl>()) { ensureInterpreter(); }

EmbeddedPyInference::~EmbeddedPyInference() {
  py::gil_scoped_acquire gil;
  m_impl.reset();
}

void EmbeddedPyInference::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  plugin->declareProperty("PythonModule", this->m_pythonModule);
  plugin->declareProperty("EntryPoint", this->m_entryPoint);
}

void EmbeddedPyInference::initialize() {
  py::gil_scoped_acquire gil;

  if (m_pythonModule.empty()) {
    throw std::runtime_error("EmbeddedPyInference: PythonModule property is not set");
  }

  try {
    py::module_ mod = py::module_::import(m_pythonModule.c_str());
    if (!py::hasattr(mod, m_entryPoint.c_str())) {
      throw std::runtime_error("EmbeddedPyInference: module '" + m_pythonModule +
                               "' does not expose a top-level callable named '" + m_entryPoint + "'");
    }
    m_impl->callable = mod.attr(m_entryPoint.c_str());

  } catch (py::error_already_set& e) {
    const std::string full = formatPyError(e);
    dd4hep::printout(dd4hep::ERROR, "EmbeddedPyInference::initialize", "Python error during initialisation:\n%s",
                     full.c_str());
    throw std::runtime_error("EmbeddedPyInference init: " + full);
  }
}

void EmbeddedPyInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
                                       std::vector<float>& output) {
  py::gil_scoped_acquire gil;

  const size_t N = inputs.size();
  if (m_impl->inputArrays.size() != N) {
    m_impl->inputArrays.clear();
    m_impl->inputArrays.reserve(N);
    m_impl->lastDataPtrs.assign(N, nullptr);
    m_impl->lastShapes.assign(N, {});
    m_impl->pyInputs = py::list(N);
    for (size_t i = 0; i < N; ++i) {
      m_impl->inputArrays.emplace_back();
    }
  }

  for (size_t i = 0; i < N; ++i) {
    const float* ptr = inputs[i].data();
    std::vector<ssize_t> shape(tensDims[i].begin(), tensDims[i].end());
    const bool needRebuild = (ptr != m_impl->lastDataPtrs[i]) || (shape != m_impl->lastShapes[i]);
    if (needRebuild) {
      m_impl->inputArrays[i] = py::array_t<float>(shape, {}, const_cast<float*>(ptr), py::none());
      m_impl->pyInputs[i] = m_impl->inputArrays[i];
      m_impl->lastDataPtrs[i] = ptr;
      m_impl->lastShapes[i] = std::move(shape);
    }
  }

  try {
    auto result = m_impl->callable(m_impl->pyInputs).cast<py::array_t<float, py::array::c_style>>();

    if (static_cast<size_t>(result.size()) != output.size()) {
      throw std::runtime_error("EmbeddedPyInference: Python returned " + std::to_string(result.size()) +
                               " floats but caller pre-allocated " + std::to_string(output.size()));
    }
    std::memcpy(output.data(), result.data(), output.size() * sizeof(float));
  } catch (py::error_already_set& e) {
    const std::string full = formatPyError(e);
    dd4hep::printout(dd4hep::ERROR, "EmbeddedPyInference::runInference", "Python error:\n%s", full.c_str());
    throw std::runtime_error("EmbeddedPyInference run: " + full);
  }
}

} // namespace ddml
