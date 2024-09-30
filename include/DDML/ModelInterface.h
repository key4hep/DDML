#ifndef ModelInterface_H
#define ModelInterface_H

#include <vector>

#include "DDML/DDML.h"

class G4FastTrack;

#include <G4ThreeVector.hh>

namespace ddml {

/** The basic interface for any model run in DDML
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class ModelInterface {
public:
  virtual ~ModelInterface(){};

  /** prepare the input vector and resize the output vector for this model
   *  based on the current FastTrack (e.g. extract kinetic energy and incident
   *  angles.) and the direction in the local coordinate system (see
   * @GeometryInterface)
   */

  virtual void prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                            TensorDimVecs& tensDims, std::vector<float>& output) = 0;

  /** interpreting the model output and create a vector of spacepoints per layer
   * in local coordinates - with the origin at the entry point into the
   * calorimeter.
   */
  virtual void convertOutput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir,
                             const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) = 0;
};

} // namespace ddml

#endif
