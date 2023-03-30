#ifndef TorchInference_H
#define TorchInference_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include <torch/script.h>

#include <memory>                              // for unique_ptr
#include <vector>                              // for vector
#include <string>                              // for string

namespace ddml {

/** Running the inference with ONXX - assuming a simple input and output vector 
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class TorchInference : public InferenceInterface {
    
  public:
    TorchInference();
    
    virtual ~TorchInference(){};
    
    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) ;
  

    /// initialize the model - based on defined properties
    void initialize() ;

    /// run the inference model - based on input vector and resized outputvector
    virtual void runInference(std::vector<float>& input,
			      std::vector<float>& output ) ;


  
  private:
    torch::jit::script::Module fModule;
    torch::TensorOptions m_options{};

    bool _isInitialized = false ;

    /// torch properties for plugin
    std::string modelPath={} ;
    int profileFlag=0;
    int optimizeFlag=0;
    int intraOpNumThreads=0;
  
  };

} // namespace
#endif
