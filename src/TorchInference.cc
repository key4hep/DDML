#include "DDML/TorchInference.h"

#define DEBUGPRINT 0

namespace ddml {

  TorchInference::TorchInference() {

  }

/// declare the proerties needed for the plugin
  void TorchInference::declareProperties( dd4hep::sim::Geant4Action* plugin ) {

    plugin->declareProperty("ModelPath" , this->modelPath ) ;
    plugin->declareProperty("ProfileFlag" , this->profileFlag ) ;
    plugin->declareProperty("OptimizeFlag" , this->optimizeFlag ) ;
    plugin->declareProperty("IntraOpNumThreads" , this->intraOpNumThreads ) ;
   
  }


  void TorchInference::initialize(){

    fModule = torch::jit::load( modelPath );
    fModule.to(torch::kCPU);
    fModule.eval();
    
    m_options = torch::TensorOptions()
      .dtype(torch::kFloat32)
      .device(torch::kCPU);
      
  }


/// run the inference model 
  void TorchInference::runInference(const std::vector<float>& input,
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
    std::vector<int64_t> dimsE = { 1, 1 , 1 , 1 ,1 };

     // create tensors with correct shape for model
    std::vector<float> latent(100) ;
    for(unsigned i=0 ; i < 100 ; ++i )
      latent[i] = input[i] ;
	  
    torch::Tensor genTensor = torch::tensor( latent , m_options).view( dimsG ) ; //{1, 100, 1, 1, 1});
    torch::Tensor ETensor = torch::tensor( input[100], m_options).view( dimsE ); //{1, 1, 1, 1, 1});
    
    if(DEBUGPRINT) {
      std::cout << " genTensor : " <<  genTensor  << std::endl ;
      std::cout << " ETensor : " << ETensor << std::endl ;

    }

    at::Tensor outTensor = fModule.forward({genTensor, ETensor}).toTensor();//.contiguous();

    if(DEBUGPRINT) {
      std::cout << " outTensor : " <<  outTensor  << std::endl ;
    }

    //torch.flatten(outTensor);
    //std::cout << "**" << outTensor << std::endl;
    //std::vector<float> output( outTensor.data_ptr<float>(), outTensor.data_ptr<float>() + outTensor.numel() );

    for(int i = 0, N=output.size() ; i < N ; ++i){
      output[i] = *(outTensor.data_ptr<float>() + i ) ;
    }

  }
} // namespace
