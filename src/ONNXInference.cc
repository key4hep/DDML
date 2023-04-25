#include "DDML/ONNXInference.h"

#include <cassert>

#define DEBUGPRINT 0


namespace ddml {

  ONNXInference::ONNXInference() {

  }

/// declare the proerties needed for the plugin
  void ONNXInference::declareProperties( dd4hep::sim::Geant4Action* plugin ) {

    plugin->declareProperty("ModelPath" , this->modelPath ) ;
    plugin->declareProperty("ProfileFlag" , this->profileFlag ) ;
    plugin->declareProperty("OptimizeFlag" , this->optimizeFlag ) ;
    plugin->declareProperty("IntraOpNumThreads" , this->intraOpNumThreads ) ;
   
  }


  void ONNXInference::initialize(){

    // initialization of the enviroment and inference session
    auto envLocal = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "ENV");
    fEnv          = std::move(envLocal);
    fSessionOptions.SetIntraOpNumThreads(intraOpNumThreads);
    // graph optimizations of the model
    // if the flag is not set to true none of the optimizations will be applied
    // if it is set to true all the optimizations will be applied
    if(optimizeFlag)
    {
      fSessionOptions.SetOptimizedModelFilePath("opt-graph");
      fSessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
      // ORT_ENABLE_BASIC #### ORT_ENABLE_EXTENDED
    }
    else
      fSessionOptions.SetGraphOptimizationLevel(ORT_DISABLE_ALL);
    // save json file for model execution profiling
    if(profileFlag)
      fSessionOptions.EnableProfiling("opt.json");
  
    auto sessionLocal = std::make_unique<Ort::Session>(*fEnv, modelPath.c_str(), fSessionOptions);
    fSession          = std::move(sessionLocal);
    fInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemTypeDefault);

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
    size_t num_input_nodes = fSession->GetInputCount();
    std::vector<const char*> input_node_names(num_input_nodes);
    for(std::size_t i = 0; i < num_input_nodes; i++)
    {
#if ORT_API_VERSION < 13
      const auto input_name = AllocatedStringPtr(fSession->GetInputName(i, allocator), allocDeleter).release();
#else
      const auto input_name               = fSession->GetInputNameAllocated(i, allocator).release() ;
#endif

      if(DEBUGPRINT) std::cout << " *** input_name : " << i << " = " << input_name << std::endl ;
      fInames.push_back( input_name ) ;
      input_node_names[i]            = input_name;
      Ort::TypeInfo type_info        = fSession->GetInputTypeInfo(i);
      auto tensor_info               = type_info.GetTensorTypeAndShapeInfo();
      input_node_dims                = tensor_info.GetShape();
      if(DEBUGPRINT) std::cout << " *** input_node_dims : " << i << " = " << input_node_dims.size() << std::endl ;
      for(std::size_t j = 0; j < input_node_dims.size(); j++)
      {
	if(input_node_dims[j] < 0)
	  input_node_dims[j] = 1;
	if(DEBUGPRINT) std::cout << "     - dim " << j << " : " << input_node_dims[j] << std::endl ;
      }
    }
    // output nodes
    std::vector<int64_t> output_node_dims;
    size_t num_output_nodes = fSession->GetOutputCount();
    std::vector<const char*> output_node_names(num_output_nodes);
    for(std::size_t i = 0; i < num_output_nodes; i++)
    {
#if ORT_API_VERSION < 12
      const auto output_name = AllocatedStringPtr(fSession->GetOutputName(i, allocator), allocDeleter).release();
#else
      const auto output_name              = fSession->GetOutputNameAllocated(i, allocator).release() ;
#endif
      fOnames.push_back( output_name );
      output_node_names[i]           = output_name;
      if(DEBUGPRINT) std::cout << " *** output_name : " << i << " = " << output_name << std::endl ;
      Ort::TypeInfo type_info        = fSession->GetOutputTypeInfo(i);
      auto tensor_info               = type_info.GetTensorTypeAndShapeInfo();
      output_node_dims               = tensor_info.GetShape();
      if(DEBUGPRINT) std::cout << " *** output_node_dims : " << i << " = " << output_node_dims.size() << std::endl ;
      for(std::size_t j = 0; j < output_node_dims.size(); j++)
      {
	if(output_node_dims[j] < 0)
	  output_node_dims[j] = 1;
	if(DEBUGPRINT) std::cout << "     - dim " << j << " : " << output_node_dims[j] << std::endl ;
      }
    }

  }


  /// run the inference model
  void ONNXInference::runInference(const std::vector<float>& input,
				   std::vector<float>& output ) {

    if( ! _isInitialized ){
      initialize() ;
      _isInitialized = true ;
    }


    // --- batch_size = 1 
    // --- noise = torch.FloatTensor(batch_size, 100, 1, 1, 1).uniform_(-1, 1).detach() 
    // --- gen_labels = np.random.uniform(10, 100, batch_size)
    // --- gen_labels = torch.FloatTensor(gen_labels)
    // --- gen_labels = gen_labels.view(batch_size, 1, 1, 1, 1).detach()
  

    // create input tensor object from data values
    std::vector<int64_t> dimsG = { 1, 100, 1 , 1 ,1 };

    Ort::Value Input_noise_tensor =
      Ort::Value::CreateTensor<float>(fInfo, const_cast<float*>(input.data()), 100 , dimsG.data(), dimsG.size());

    assert(Input_noise_tensor.IsTensor());

    std::vector<int64_t> dimsE = { 1, 1 , 1 , 1 ,1 };

    Ort::Value Input_energy_tensor =
      Ort::Value::CreateTensor<float>(fInfo, const_cast<float*>(&input[100]) , 1 , dimsE.data(), dimsE.size());

    assert(Input_energy_tensor.IsTensor());

    if(DEBUGPRINT) std::cout << " ONNXInference::runInference: Input_energy_tensor : " <<  input[100]  << std::endl ;


    std::vector<Ort::Value> ort_inputs;
    ort_inputs.push_back(std::move(Input_noise_tensor));
    ort_inputs.push_back(std::move(Input_energy_tensor));

    // run the inference session
    std::vector<Ort::Value> ort_outputs =
      fSession->Run(Ort::RunOptions{ nullptr }, fInames.data(), ort_inputs.data(), ort_inputs.size(),
		    fOnames.data(), fOnames.size());



    // get pointer to output tensor float values
    const auto* floatarr = ort_outputs.front().GetTensorData<float>();
//  output.assign(outputSize, 0);
    for(int i = 0, N=output.size() ; i < N ; ++i){
      output[i] = floatarr[i];
      //    if(DEBUGPRINT) std::cout << " e = " << output[i] << ", " ;
    }
    //if(DEBUGPRINT) std::cout << std::endl ;


  }
} // namespace
