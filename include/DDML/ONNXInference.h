#ifndef ONNXInference_H
#define ONNXInference_H

#include "DDML/InferenceInterface.h"
#include <DDG4/Geant4Action.h>

#include "core/session/onnxruntime_cxx_api.h"  // for Env, Session, SessionO...

#include <memory>                              // for unique_ptr
#include <vector>                              // for vector
#include <string>                              // for string

namespace ddml {

/** Running the inference with ONXX - assuming a simple input and output vector 
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class ONNXInference : public InferenceInterface {
    
  public:
    ONNXInference();
    
    virtual ~ONNXInference(){};
    
    /// declare the proerties needed for the plugin
    void declareProperties( dd4hep::sim::Geant4Action* plugin ) ;
  

    /// initialize the model - based on defined properties
    void initialize() ;

    /// run the inference model - based on input vector and resized outputvector
    virtual void runInference(const std::vector<float>& input,
			      std::vector<float>& output ) ;


  
  private:
    /// Pointer to the ONNX enviroment
    std::unique_ptr<Ort::Env> fEnv;
    /// Pointer to the ONNX inference session
    std::unique_ptr<Ort::Session> fSession;
    /// ONNX settings
    Ort::SessionOptions fSessionOptions;
    /// ONNX memory info
    const OrtMemoryInfo* fInfo;
    struct MemoryInfo;
    /// the input names represent the names given to the model
    /// when defining  the model's architecture (if applicable)
    /// they can also be retrieved from model.summary()
    std::vector<const char*> fInames;
    std::vector<const char*> fOnames;

    bool _isInitialized = false ;


    /// onxx properties for plugin
    std::string modelPath={} ;
    int profileFlag=0;
    int optimizeFlag=0;
    int intraOpNumThreads=0;
  
  };

} // namespace
#endif
