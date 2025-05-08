#ifndef LOADHDF5IO_H
#define LOADHDF5IO_H

#include "DDML/CaloCloudsTwoAngleModel.h"

namespace ddml {

/** 
 *  Inherits from CaloCloudsTwoAngleModel, but instead of 
 *  energy+angles, prepares the input vector to be
 *  true MC momentum (px,py,pz).
 *  
 *  @author A.Korol, DESY
 *  @date April. 2025
 */
class LoadHdf5IO : public CaloCloudsTwoAngleModel {
public:
LoadHdf5IO() = default;
~LoadHdf5IO() override = default;


  /// Only override prepareInput; convertOutput() is inherited as‐is.
  void prepareInput(G4FastTrack const& aFastTrack,
                    G4ThreeVector const& localDir,
                    InputVecs&         inputs,
                    TensorDimVecs&     tensDims,
                    std::vector<float>& output) override;

};

} // namespace ddml

#endif
