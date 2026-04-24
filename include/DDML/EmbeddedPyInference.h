#ifndef EmbeddedPyInference_H
#define EmbeddedPyInference_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include <memory>
#include <string>
#include <vector>

namespace ddml {

/** Inference backend that runs a user-supplied Python callable inside an
 *  embedded CPython interpreter. The user module must expose a top-level
 *  callable (default name `run_inference`) with the contract
 *
 *      run_inference(inputs: list[np.ndarray]) -> np.ndarray  # flat float32
 *
 *  Initialisation is eager — done at Geant4/DD4hep setup time via the
 *  FastMLShower lifecycle hook — not lazy on first runInference call.
 *
 *  Thread safety: the embedded interpreter is process-global and all calls
 *  serialise under the GIL. Not suitable for Geant4 MT scaling beyond what
 *  the GIL permits.
 */
class EmbeddedPyInference {
public:
  EmbeddedPyInference();
  ~EmbeddedPyInference();

  EmbeddedPyInference(const EmbeddedPyInference&) = delete;
  EmbeddedPyInference& operator=(const EmbeddedPyInference&) = delete;

  void declareProperties(dd4hep::sim::Geant4Action* plugin);
  void initialize();
  void runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output);

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;

  // DD4hep properties
  std::string m_pythonPath = {};          // ':'-separated, prepended to sys.path
  std::string m_pythonModule = {};        // e.g. "cc3_sf_2a_wrapper"
  std::string m_modelPath = {};           // exported via env DDML_MODEL_PATH
  std::string m_entryPoint = "run_inference"; // callable name inside the module
  int m_intraOpNumThreads = 0;            // forwarded to the Python side
  bool m_isInitialized = false;
};

static_assert(InferenceInterface<EmbeddedPyInference>,
              "EmbeddedPyInference must satisfy InferenceInterface");

} // namespace ddml
#endif
