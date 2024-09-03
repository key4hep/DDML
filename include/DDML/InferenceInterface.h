#ifndef InferenceInterface_H
#define InferenceInterface_H

#include "DDML/DDML.h"

#include <vector>

namespace ddml {

/** The basic interface for running inference with one input vector and one
 * output vector.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class InferenceInterface {
public:
  virtual ~InferenceInterface(){};

  /// run the inference model - based on input vector and resized outputvector
  virtual void runInference(const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output) = 0;
};

} // namespace ddml

#endif
