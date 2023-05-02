#include "DDML/TorchInference.h"
#include <cassert>

#define DEBUGPRINT 1

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
  void TorchInference::runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
				   std::vector<float>& output ) {

    if( ! _isInitialized ){
      initialize() ;
      _isInitialized = true ;
    }

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

    }
   
    assert( inputs.size() == tensDims.size() ) ;

    std::vector<at::IValue> tensors ;

    size_t nIn = inputs.size() ;
    for(unsigned i=0,N = inputs.size(); i<N ; ++i ){

      torch::Tensor inTens = torch::tensor( inputs[i] , m_options).view( tensDims[i] ) ;
      tensors.emplace_back( inTens ) ;
    }


    at::Tensor outTensor = fModule.forward( tensors  ).toTensor();//.contiguous();

    //torch.flatten(outTensor);
    //std::cout << "**" << outTensor << std::endl;
    //std::vector<float> output( outTensor.data_ptr<float>(), outTensor.data_ptr<float>() + outTensor.numel() );

    for (auto out : std::vector<float>(outTensor.data_ptr<float>(), outTensor.data_ptr<float>() +outTensor.numel())) {

            output.push_back(out);
    }
 
  }
} // namespace
