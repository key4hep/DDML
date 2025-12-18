#ifndef InferenceInterface_H
#define InferenceInterface_H

#include "DDML/DDML.h"

#include <vector>
#include <concepts>

namespace ddml {

/** The basic concept for running inference with one input vector and one
 * output vector.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

template<typename T>
concept InferenceInterface = requires(T t, const InputVecs& inputs, const TensorDimVecs& tensDims, std::vector<float>& output) {
  /// run the inference model - based on input vector and resized outputvector
  t.runInference(inputs, tensDims, output);
};

} // namespace ddml

#endif
