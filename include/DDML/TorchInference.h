#ifndef TorchInference_H
#define TorchInference_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include <torch/script.h>

#include <memory> // for unique_ptr
#include <string> // for string
#include <vector> // for vector

namespace ddml {

/** Running the inference with ONXX - assuming a simple input and output vector
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class TorchInference {
public:
  TorchInference();

  virtual ~TorchInference() = default;

  /// declare the proerties needed for the plugin
  void declareProperties(dd4hep::sim::Geant4Action* plugin);

  /// initialize the model - based on defined properties
  void initialize();

  /// run the inference model - based on input vector and resized outputvector
  void runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output);

private:
  torch::jit::script::Module m_jitModule;
  torch::TensorOptions m_options{};

  /// torch properties for plugin
  std::string m_modelPath = {};
  int m_profileFlag = 0;
  int m_optimizeFlag = 0;
  int m_intraOpNumThreads = 0;
};

static_assert(InferenceInterface<TorchInference>, "TorchInference must satisfy InferenceInterface concept");

} // namespace ddml
#endif
