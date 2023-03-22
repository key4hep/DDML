#ifndef InferenceInterface_H
#define InferenceInterface_H

#include <vector>

/** The basic interface for running inference with one input vector and one output vector.
 *
 */

class InferenceInterface {
public:
  virtual ~InferenceInterface(){};
  
  /// run the inference model 
  virtual void runInference(const std::vector<float>& input,
			    std::vector<float>& output,
			    int outputSize) = 0;
};

#endif
