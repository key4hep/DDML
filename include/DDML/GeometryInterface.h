#ifndef GeometryInterface_H
#define GeometryInterface_H

#include <vector>

#include "DDML/DDML.h"

class G4FastTrack ;

namespace ddml {

/** The basic interface for global geometry
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */
  
  class GeometryInterface {

  public:
    virtual ~GeometryInterface(){};
    

    /** convert the local spacepoints to global spacepoints
     */
    virtual void localToGlobal(G4FastTrack const& aFastTrack,
			       std::vector<SpacePointVec>& spacepoints ) = 0;
    
    
  };

} // namespace

#endif
