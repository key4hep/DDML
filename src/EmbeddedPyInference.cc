#include "DDML/EmbeddedPyInference.h"

#include <algorithm>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <string>

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
    std::call_once(g_interpInit, [] {
      if (Py_IsInitialized()) {
        // Host (e.g. ddsim running via cppyy) already owns the interpreter;
        // do not create a second one.
        return;
      }
      // Process-lifetime interpreter, intentionally never finalised:
      // finalising CPython with NumPy (or torch) loaded is unreliable, and
      // plugins may still be destroyed after main() returns.
      py::initialize_interpreter();
      // initialize_interpreter() leaves the calling thread holding the GIL.
      // Release it so that other threads (e.g. Geant4 MT workers) can take it
      // via gil_scoped_acquire. The saved thread state is intentionally leaked.
      PyEval_SaveThread();
    });
  }

  /// Whether it is still safe to touch Python objects (take the GIL, decref)
  bool interpreterAlive() {
#if PY_VERSION_HEX >= 0x030D0000
    return Py_IsInitialized() && !Py_IsFinalizing();
#else
    return Py_IsInitialized() && !_Py_IsFinalizing();
#endif
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

EmbeddedPyInference::EmbeddedPyInference() {
  // The interpreter has to exist and we need the GIL before creating any
  // Python objects (Impl holds a py::list)
  ensureInterpreter();
  py::gil_scoped_acquire gil;
  m_impl = std::make_unique<Impl>();
}

EmbeddedPyInference::~EmbeddedPyInference() {
  if (!m_impl) {
    return;
  }
  if (!interpreterAlive()) {
    // The interpreter has already been (or is being) torn down, e.g. by the
    // host. The Python objects are gone with it, so drop the handle without
    // touching its reference count.
    m_impl->callable.release();
    m_impl.reset();
    return;
  }
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

  dd4hep::printout(dd4hep::INFO, "EmbeddedPyInference::initialize", "Using python module '%s.%s' for running inference",
                   m_pythonModule.c_str(), m_entryPoint.c_str());
}

void EmbeddedPyInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
                                       std::vector<float>& output) {
  const size_t N = inputs.size();
  // The inputs are handed to python as zero-copy views using these shapes, so
  // a mismatch would make numpy read past the end of the input buffers
  if (tensDims.size() != N) {
    throw std::runtime_error("EmbeddedPyInference: got " + std::to_string(N) + " input vectors but " +
                             std::to_string(tensDims.size()) + " tensor shapes");
  }
  for (size_t i = 0; i < N; ++i) {
    int64_t nElements = 1;
    for (const auto dim : tensDims[i]) {
      if (dim < 0) {
        throw std::runtime_error("EmbeddedPyInference: negative dimension in tensor shape for input " +
                                 std::to_string(i));
      }
      nElements *= dim;
    }
    if (static_cast<size_t>(nElements) != inputs[i].size()) {
      throw std::runtime_error("EmbeddedPyInference: tensor shape for input " + std::to_string(i) + " describes " +
                               std::to_string(nElements) + " elements but the input vector has " +
                               std::to_string(inputs[i].size()));
    }
  }

  py::gil_scoped_acquire gil;

  dd4hep::printout(dd4hep::DEBUG, "EmbeddedPyInference::runInference", "running inference with %zu input vectors", N);

  // Wrap the inputs as zero-copy views. Passing a base object stops numpy from
  // copying the data or taking ownership of it.
  py::list pyInputs(N);
  for (size_t i = 0; i < N; ++i) {
// GCC sees through to the static None object used as base and warns about the
// (dead) branch in pybind11 that reads the flags of base if it is an array
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
    pyInputs[i] = py::array_t<float>(tensDims[i], {}, const_cast<float*>(inputs[i].data()), py::none());
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
  }

  try {
    dd4hep::printout(dd4hep::DEBUG, "EmbeddedPyInference::runInference", "Calling python inference now");
    py::object pyResult = m_impl->callable(pyInputs);
    // pybind11 happily converts None into a 0-d array holding NaN, which would
    // silently corrupt the output. A None result is almost certainly a missing
    // return statement, so make it a proper Python error instead.
    if (pyResult.is_none()) {
      const std::string msg = "'" + m_pythonModule + "." + m_entryPoint +
                              "' returned None, expected a float32 array (missing return statement?)";
      PyErr_SetString(PyExc_TypeError, msg.c_str());
      throw py::error_already_set();
    }
    auto result = pyResult.cast<py::array_t<float, py::array::c_style>>();
    dd4hep::printout(dd4hep::DEBUG, "EmbeddedPyInference::runInference",
                     "After python inference. result has %zd elements", result.size());

    // Caller pre-sizes `output` to the model's worst-case footprint. The actual
    // model output may be smaller (self-describing, variable-length payload);
    // convertOutput reads only the meaningful prefix. Trailing bytes stay at
    // the zero-fill from prepareInput().
    // Copy values back into the outputs making sure that we neither overrun the
    // output buffer nor that we read past the end of the output tensor. We can
    // ignore everyting in output past n, because we clear it for every run in
    // FastMLShower::modelShower
    const auto tensorSize = static_cast<size_t>(result.size());
    if (tensorSize > output.size()) {
      // We could technically just make enough room, but that might be hiding a
      // configuration or model issue as it is the models responsibility to size
      // this vector
      dd4hep::printout(dd4hep::WARNING, "EmbeddedPyInference::runInference",
                       "model returned more values than were pre-allocated");
    }
    const size_t n = std::min(tensorSize, output.size());
    std::memcpy(output.data(), result.data(), n * sizeof(float));
  } catch (py::error_already_set& e) {
    const std::string full = formatPyError(e);
    dd4hep::printout(dd4hep::ERROR, "EmbeddedPyInference::runInference", "Python error:\n%s", full.c_str());
    throw std::runtime_error("EmbeddedPyInference run: " + full);
  }
}

} // namespace ddml
