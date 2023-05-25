#ifndef TorchInference_H
#define TorchInference_H


#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#undef ClassDef
#include <torch/script.h>
#define ClassDef(name,id)                            \
   _ClassDefOutline_(name,id,virtual,)               \
   static int DeclFileLine() { return __LINE__; }

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
    virtual void runInference(const InputVecs& inputs, const TensorDimVecs& tensDims,
			      std::vector<float>& output ) ;

    /// setter for the model path, so that is accessible through python bindings.
    void setModelPath(const std::string &modelPath) {
          TorchInference::modelPath = modelPath;
      }

    /// getter for tensor options used in python to cpp test suite
    const torch::TensorOptions &getMOptions() const {
      return m_options;
    }

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
