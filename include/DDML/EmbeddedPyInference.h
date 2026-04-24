#ifndef EmbeddedPyInference_H
#define EmbeddedPyInference_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include <string>
#include <vector>

namespace ddml {

/** Inference backend that runs a user-supplied PyTorch model inside an
 *  embedded CPython interpreter. The user module must expose a
 *  top-level callable named `run_inference(inputs)` returning a
 *  flat float32 numpy array.
 *
 *  Initialisation is eager — done at Geant4/DD4hep setup time via the
 *  FastMLShower lifecycle hook — not lazy on first runInference call.
 */
// pybind11 marks its types with hidden visibility; suppress the GCC -Wattributes
// warning that fires when a default-visibility class holds such a member.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
class EmbeddedPyInference {
public:
  EmbeddedPyInference();
  ~EmbeddedPyInference();

  void declareProperties(dd4hep::sim::Geant4Action* plugin);
  void initialize();
  void runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output);

private:
  // The global interpreter guard lives in the .cc translation unit;
  // per-instance we only need the callable handle.
  pybind11::object m_callable;

  bool m_isInitialized = false;

  // DD4hep properties
  std::string m_pythonPath = {};   // ':'-separated, prepended to sys.path
  std::string m_pythonModule = {}; // e.g. "cc3_sf_2a_wrapper"
  std::string m_modelPath = {};    // exported via env DDML_MODEL_PATH
  int m_intraOpNumThreads = 0;     // forwarded to torch.set_num_threads
};

#pragma GCC diagnostic pop

static_assert(InferenceInterface<EmbeddedPyInference>, "EmbeddedPyInference must satisfy InferenceInterface");

} // namespace ddml
#endif
