#include "DDML/ONNXInference.h"

#include <cassert>
#include <iostream>

#define DEBUGPRINT 0

namespace ddml {

ONNXInference::ONNXInference() {
}

/// declare the proerties needed for the plugin
void ONNXInference::declareProperties(dd4hep::sim::Geant4Action* plugin) {
  plugin->declareProperty("ModelPath", this->m_modelPath);
  plugin->declareProperty("ProfileFlag", this->m_profileFlag);
  plugin->declareProperty("OptimizeFlag", this->m_optimizeFlag);
  plugin->declareProperty("IntraOpNumThreads", this->m_intraOpNumThreads);
}

void ONNXInference::initialize() {
  // initialization of the enviroment and inference session
  auto envLocal = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "ENV");
  m_env = std::move(envLocal);
  m_sessionOptions.SetIntraOpNumThreads(m_intraOpNumThreads);
  // graph optimizations of the model
  // if the flag is not set to true none of the optimizations will be applied
  // if it is set to true all the optimizations will be applied
  if (m_optimizeFlag) {
    m_sessionOptions.SetOptimizedModelFilePath("opt-graph");
    m_sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
    // ORT_ENABLE_BASIC #### ORT_ENABLE_EXTENDED
  } else {
    m_sessionOptions.SetGraphOptimizationLevel(ORT_DISABLE_ALL);
  }
  // save json file for model execution profiling
  if (m_profileFlag) {
    m_sessionOptions.EnableProfiling("opt.json");
  }

  auto sessionLocal = std::make_unique<Ort::Session>(*m_env, m_modelPath.c_str(), m_sessionOptions);
  m_session = std::move(sessionLocal);
  m_memInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemTypeDefault);

  // Collect information about input parameter shapes and names that are
  // necessary to run inference.
  // input nodes
  Ort::AllocatorWithDefaultOptions allocator;
#if ORT_API_VERSION < 13
  // Before 1.13 we have to roll our own unique_ptr wrapper here
  auto allocDeleter = [&allocator](char* p) { allocator.Free(p); };
  using AllocatedStringPtr = std::unique_ptr<char, decltype(allocDeleter)>;
#endif
  std::vector<int64_t> input_node_dims;
  size_t num_input_nodes = m_session->GetInputCount();
  std::vector<const char*> input_node_names(num_input_nodes);
  for (std::size_t i = 0; i < num_input_nodes; i++) {
#if ORT_API_VERSION < 13
    const auto input_name = AllocatedStringPtr(m_session->GetInputName(i, allocator), allocDeleter).release();
#else
    const auto input_name = m_session->GetInputNameAllocated(i, allocator).release();
#endif

    if (DEBUGPRINT) {
      std::cout << " *** input_name : " << i << " = " << input_name << std::endl;
    }
    m_inNames.push_back(input_name);
    input_node_names[i] = input_name;
    Ort::TypeInfo type_info = m_session->GetInputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    input_node_dims = tensor_info.GetShape();
    if (DEBUGPRINT) {
      std::cout << " *** input_node_dims : " << i << " = " << input_node_dims.size() << std::endl;
    }
    for (std::size_t j = 0; j < input_node_dims.size(); j++) {
      if (input_node_dims[j] < 0) {
        input_node_dims[j] = 1;
      }
      if (DEBUGPRINT) {
        std::cout << "     - dim " << j << " : " << input_node_dims[j] << std::endl;
      }
    }
  }
  // output nodes
  std::vector<int64_t> output_node_dims;
  size_t num_output_nodes = m_session->GetOutputCount();
  std::vector<const char*> output_node_names(num_output_nodes);
  for (std::size_t i = 0; i < num_output_nodes; i++) {
#if ORT_API_VERSION < 12
    const auto output_name = AllocatedStringPtr(m_session->GetOutputName(i, allocator), allocDeleter).release();
#else
    const auto output_name = m_session->GetOutputNameAllocated(i, allocator).release();
#endif
    m_outNames.push_back(output_name);
    output_node_names[i] = output_name;
    if (DEBUGPRINT) {
      std::cout << " *** output_name : " << i << " = " << output_name << std::endl;
    }
    Ort::TypeInfo type_info = m_session->GetOutputTypeInfo(i);
    auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
    output_node_dims = tensor_info.GetShape();
    if (DEBUGPRINT) {
      std::cout << " *** output_node_dims : " << i << " = " << output_node_dims.size() << std::endl;
    }
    for (std::size_t j = 0; j < output_node_dims.size(); j++) {
      if (output_node_dims[j] < 0) {
        output_node_dims[j] = 1;
      }
      if (DEBUGPRINT) {
        std::cout << "     - dim " << j << " : " << output_node_dims[j] << std::endl;
      }
    }
  }
}

/// run the inference model
void ONNXInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output) {
  if (!m_isInitialized) {
    initialize();
    m_isInitialized = true;
  }

  // create input tensor object from data values

  assert(inputs.size() == tensDims.size());

  std::vector<Ort::Value> ort_inputs;

  for (unsigned i = 0, N = inputs.size(); i < N; ++i) {
    Ort::Value tensor = Ort::Value::CreateTensor<float>(m_memInfo, const_cast<float*>(inputs[i].data()),
                                                        inputs[i].size(), tensDims[i].data(), tensDims[i].size());

    assert(tensor.IsTensor());

    ort_inputs.push_back(std::move(tensor));
  }

  // run the inference session
  std::vector<Ort::Value> ort_outputs = m_session->Run(Ort::RunOptions{nullptr}, m_inNames.data(), ort_inputs.data(),
                                                       ort_inputs.size(), m_outNames.data(), m_outNames.size());

  // get pointer to output tensor float values
  const auto* floatarr = ort_outputs.front().GetTensorData<float>();
  //  output.assign(outputSize, 0);
  for (int i = 0, N = output.size(); i < N; ++i) {
    output[i] = floatarr[i];
    //    if(DEBUGPRINT) std::cout << " e = " << output[i] << ", " ;
  }
  // if(DEBUGPRINT) std::cout << std::endl ;
}
} // namespace ddml
