#ifndef ModelInterface_H
#define ModelInterface_H

#include <vector>

#include "DDML/DDML.h"

class G4FastTrack ;

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
     *  angles.)
     */
    
    virtual void prepareInput(G4FastTrack const& aFastTrack,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) = 0;


    /** create a vector of spacepoints per layer interpreting the model output  
     */
    virtual void convertOutput(G4FastTrack const& aFastTrack,
			       const std::vector<float>& output,
			       std::vector<SpacePointVec>& spacepoints ) = 0;
    
    
  };

} // namespace

#endif
