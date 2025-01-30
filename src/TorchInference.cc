#include "DDML/TorchInference.h"
#include "omp.h" // for setting num torch threads
#include <cassert>

#define DEBUGPRINT 0

namespace ddml {

TorchInference::TorchInference() {
}

/// declare the proerties needed for the plugin
void TorchInference::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  plugin->declareProperty("ModelPath", this->m_modelPath);
  plugin->declareProperty("ProfileFlag", this->m_profileFlag);
  plugin->declareProperty("OptimizeFlag", this->m_optimizeFlag);
  plugin->declareProperty("IntraOpNumThreads", this->m_intraOpNumThreads);
}

void TorchInference::initialize() {
  c10::InferenceMode guard(true);

  m_jitModule = torch::jit::load(m_modelPath);
  m_jitModule.to(torch::kCPU);
  m_jitModule.eval();

  m_options = torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU);

  // set number of threads
  omp_set_num_threads(m_intraOpNumThreads);

  int num_threads = omp_get_num_threads();
  int max_threads = omp_get_max_threads();

  dd4hep::printout(dd4hep::DEBUG, "TorchInference::initialize", "omp num threads: %i", num_threads);
  dd4hep::printout(dd4hep::DEBUG, "TorchInference::initialize", "omp max threads: %i", max_threads);
}

/// run the inference model
void TorchInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output) {
  c10::InferenceMode guard(true);

  if (!m_isInitialized) {
    initialize();
    m_isInitialized = true;
  }

  if (DEBUGPRINT) {
    std::cout << " ----- TorchInference::runInference \n"
              << "    # inputs = " << inputs.size() << " : ";

    for (auto iv : inputs) {
      std::cout << " " << iv.size() << ", ";
    }

    std::cout << std::endl;

    std::cout << "    # dims = " << tensDims.size() << " : ";

    for (auto iv : tensDims) {
      std::cout << " " << iv.size() << ", ";
    }

    std::cout << std::endl;
  }

  assert(inputs.size() == tensDims.size());

  std::vector<at::IValue> tensors;

  for (unsigned i = 0, N = inputs.size(); i < N; ++i) {
    torch::Tensor inTens = torch::tensor(inputs[i], m_options).view(tensDims[i]);
    tensors.emplace_back(inTens);

    if (DEBUGPRINT) {
      std::cout << " inTensor " << i << " : " << inTens << std::endl;
    }
  }

  at::Tensor outTensor = m_jitModule.forward(tensors).toTensor(); //.contiguous();

  if (DEBUGPRINT) {
    std::cout << " outTensor : " << outTensor << std::endl;
  }

  // torch.flatten(outTensor);
  // std::cout << "**" << outTensor << std::endl;
  // std::vector<float> output( outTensor.data_ptr<float>(),
  // outTensor.data_ptr<float>() + outTensor.numel() );

  for (unsigned j = 0; j < output.size(); ++j) {
    output[j] = *(outTensor.data_ptr<float>() + j);
  }
}
} // namespace ddml
