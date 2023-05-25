#include <iostream>
#include "DDML/TorchInference.h"
#include <cassert>

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
    std::cout << "Model Path: " << modelPath << std::endl;
    fModule = torch::jit::load( modelPath );
    fModule.to(torch::kCPU);
    fModule.eval();

    m_options = torch::TensorOptions()
      .dtype(torch::kFloat32)
      .device(torch::kCPU);

  }


/// run the inference model 
  void TorchInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
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

    if(DEBUGPRINT){
      std::cout << " ----- TorchInference::runInference \n"
		<< "    # inputs = " << inputs.size() << " : " ;

      for(auto iv : inputs )
	std::cout << " " << iv.size() << ", " ;

      std::cout << std::endl ;

      std::cout << "    # dims = " << tensDims.size() << " : " ;

      for(auto iv : tensDims )
	std::cout << " " << iv.size() << ", " ;

      std::cout << std::endl ;

     // create tensors with correct shape for model
    std::vector<float> latent(100) ;
    for(unsigned i=0 ; i < 100 ; ++i )
      latent[i] = inputs[i] ;

    torch::Tensor genTensor = torch::tensor( latent , m_options).view( dimsG ) ; //{1, 100, 1, 1, 1});
    torch::Tensor ETensor = torch::tensor( inputs[100], m_options).view( dimsE ); //{1, 1, 1, 1, 1});

    if(DEBUGPRINT) {
      std::cout << " genTensor : " <<  genTensor  << std::endl ;
      std::cout << " ETensor : " << ETensor << std::endl ;

    }
   
    assert( inputs.size() == tensDims.size() ) ;

    std::vector<at::IValue> tensors ;

    size_t nIn = inputs.size() ;
    for(unsigned i=0,N = inputs.size(); i<N ; ++i ){

      torch::Tensor inTens = torch::tensor( inputs[i] , m_options).view( tensDims[i] ) ;
      tensors.emplace_back( inTens ) ;

      if(DEBUGPRINT) {
	std::cout << " inTensor " << i << " : " <<  inTens  << std::endl ;
      }
      
    }


    at::Tensor outTensor = fModule.forward( tensors  ).toTensor();//.contiguous();

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
