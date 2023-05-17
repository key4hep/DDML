#include "DDML/TorchInference.h"

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
  void TorchInference::runInference(const std::vector<float>& input,
				   std::vector<float>& output ) {

    if( ! _isInitialized ){
      initialize() ;
      _isInitialized = true ;
    }
      
    // This code is for the BIB-AE
    // --- batch_size = 1 
    //     Required INPUT:
    //     CondE: Incident energy conditioning (GeV) : 10-100GeV, Tensor[batch_size, 1]
    //     CondTheta: Incidet angle (radians): 30-90 degrees, Tensor[batch_size, 1]
    //     Cond: torch.cat((condE/100).float(), (condTheta/np.radians(90.0)).float(), 1)
    //     Input: (condE, cond_theta, cond)

    // create input tensor objects from data values
    std::vector<int64_t> dims_CondE = {1, 1};
    std::vector<int64_t> dims_CondTheta = {1,1};

    // for now, fake showers with angle 89.0 degrees (avoid edge of training space)
    std::vector<float> Theta_vec(1, 1);
    for(unsigned i=0; i<1; ++i ){
      Theta_vec[i] = 89.*(M_PI/180.);
    }

    // Input[100] is currently the incident energy in GeV

    if(DEBUGPRINT) std::cout << " Input_energy_tensor : " <<  input[100]  << std::endl ;
    

    torch::Tensor ETensor = torch::tensor( input[100], m_options ).view( dims_CondE );
    torch::Tensor ThetaTensor = torch::tensor( Theta_vec, m_options ).view( dims_CondTheta );

    torch::Tensor CondTensor = torch::cat({ ETensor/100., ThetaTensor/(90.*(M_PI/180.)) }, 1);

    at::Tensor outTensor = fModule.forward({ETensor, ThetaTensor, CondTensor}).toTensor();


        for(int i = 0, N=output.size() ; i < N ; ++i){
      output[i] = *(outTensor.data_ptr<float>() + i ) ;
	}


  /*


    // The lines below are for the GAN

    // --- batch_size = 1 
    // --- noise = torch.FloatTensor(batch_size, 100, 1, 1, 1).uniform_(-1, 1).detach() 
    // --- gen_labels = np.random.uniform(10, 100, batch_size)
    // --- gen_labels = torch.FloatTensor(gen_labels)
    // --- gen_labels = gen_labels.view(batch_size, 1, 1, 1, 1).detach()


    // create input tensor object from data values
    std::vector<int64_t> dimsG = { 1, 100, 1 , 1 ,1 };
    std::vector<int64_t> dimsE = { 1, 1 , 1 , 1 ,1 };

     // create tensors with correct shape for model
    std::vector<float> latent(100, 1) ;
    for(unsigned i=0 ; i < 100 ; ++i ){
      latent[i] = input[i] ;
    }
	  
    torch::Tensor genTensor = torch::tensor( latent , m_options).view( dimsG ) ; //{1, 100, 1, 1, 1});
    torch::Tensor ETensor = torch::tensor( input[100], m_options).view( dimsE ); //{1, 1, 1, 1, 1});
    
    if(DEBUGPRINT) std::cout << " Input_energy_tensor : " <<  input[100]  << std::endl ;
    if(DEBUGPRINT) std::cout << " Input Noise tensor : " <<  latent  << std::endl ;
    if(DEBUGPRINT) std::cout << " Output tensor pre-population : " <<  output  << std::endl ;


    at::Tensor outTensor = fModule.forward({genTensor, ETensor}).toTensor();//.contiguous();

    //if(DEBUGPRINT) std::cout << " output_tensor : " << outTensor << std::endl ; 

    //torch.flatten(outTensor);
    //std::cout << "**" << outTensor << std::endl;
    //std::vector<float> output( outTensor.data_ptr<float>(), outTensor.data_ptr<float>() + outTensor.numel() );

    
  //  for (auto out : std::vector<float>(outTensor.data_ptr<float>(), outTensor.data_ptr<float>() +outTensor.numel())) {

   //         if(DEBUGPRINT) std::cout << " Output tensor : " <<  out << std::endl ;

   //         output.push_back(out);
   // }

    for(int i = 0, N=output.size() ; i < N ; ++i){
      output[i] = *(outTensor.data_ptr<float>() + i ) ;
	}
 
  */

  }
} // namespace
