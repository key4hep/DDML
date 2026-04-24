#include "DDML/EmbeddedPyInference.h"

#include <cstdlib> // setenv
#include <cstring>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace ddml {

struct __attribute__((visibility("hidden"))) EmbeddedPyInference::Impl {
  py::object callable;
};

namespace {
  std::once_flag g_interpInit;
  void ensureInterpreter() {
    // Leaked on purpose: process-lifetime interpreter.
    // Finalising CPython with NumPy loaded is unreliable.
    std::call_once(g_interpInit, [] {
      static py::scoped_interpreter interp{};
      (void)interp;
    });
  }
} // namespace

EmbeddedPyInference::EmbeddedPyInference() : m_impl(std::make_unique<Impl>()) {
  ensureInterpreter();
}

EmbeddedPyInference::~EmbeddedPyInference() {
  py::gil_scoped_acquire gil;
  m_impl.reset();
}

void EmbeddedPyInference::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  plugin->declareProperty("PythonPath", this->m_pythonPath);
  plugin->declareProperty("PythonModule", this->m_pythonModule);
  plugin->declareProperty("ModelPath", this->m_modelPath);
  plugin->declareProperty("IntraOpNumThreads", this->m_intraOpNumThreads);
}

void EmbeddedPyInference::initialize() {
  if (m_isInitialized) {
    return;
  }

  py::gil_scoped_acquire gil;

  if (!m_modelPath.empty()) {
    ::setenv("DDML_MODEL_PATH", m_modelPath.c_str(), /*overwrite=*/1);
  }

  if (!m_pythonPath.empty()) {
    std::vector<std::string> paths;
    for (auto part : std::views::split(std::string_view{m_pythonPath}, ':')) {
      if (part.begin() != part.end())
        paths.emplace_back(part.begin(), part.end());
    }
    // Prepend in reverse so the first entry in m_pythonPath ends up first in sys.path
    py::object sysPath = py::module_::import("sys").attr("path");
    for (auto it = paths.rbegin(); it != paths.rend(); ++it) {
      sysPath.attr("insert")(0, *it);
    }
  }

  if (m_pythonModule.empty()) {
    throw std::runtime_error("EmbeddedPyInference: PythonModule property is not set");
  }

  try {
    py::module_ mod = py::module_::import(m_pythonModule.c_str());
    if (!py::hasattr(mod, "run_inference")) {
      throw std::runtime_error("EmbeddedPyInference: module '" + m_pythonModule +
                               "' does not expose a top-level callable named 'run_inference'");
    }
    m_impl->callable = mod.attr("run_inference");

    if (m_intraOpNumThreads > 0) {
      py::module_::import("torch").attr("set_num_threads")(m_intraOpNumThreads);
    }
  } catch (py::error_already_set& e) {
    dd4hep::printout(dd4hep::ERROR, "EmbeddedPyInference::initialize", "Python error during initialisation: %s",
                     e.what());
    throw std::runtime_error(std::string("EmbeddedPyInference init: ") + e.what());
  }

  m_isInitialized = true;
}

void EmbeddedPyInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
                                       std::vector<float>& output) {
  py::gil_scoped_acquire gil;

  if (!m_isInitialized) {
    initialize();
  }

  py::list pyInputs;
  for (unsigned i = 0, N = inputs.size(); i < N; ++i) {
    // Non-owning numpy view over the std::vector<float> buffer; no copy.
    std::vector<ssize_t> shape(tensDims[i].begin(), tensDims[i].end());
    pyInputs.append(py::array_t<float>(shape, {}, // strides default
                                       const_cast<float*>(inputs[i].data()), py::cast(this)));
  }

  try {
    py::array_t<float, py::array::c_style | py::array::forcecast> result =
        m_impl->callable(pyInputs).cast<py::array_t<float, py::array::c_style | py::array::forcecast>>();

    if (static_cast<size_t>(result.size()) != output.size()) {
      throw std::runtime_error("EmbeddedPyInference: Python returned " + std::to_string(result.size()) +
                               " floats but caller pre-allocated " + std::to_string(output.size()));
    }
    std::memcpy(output.data(), result.data(), output.size() * sizeof(float));
  } catch (py::error_already_set& e) {
    dd4hep::printout(dd4hep::ERROR, "EmbeddedPyInference::runInference", "Python error: %s", e.what());
    throw std::runtime_error(std::string("EmbeddedPyInference run: ") + e.what());
  }
}

} // namespace ddml
