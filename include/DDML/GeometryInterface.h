#ifndef GeometryInterface_H
#define GeometryInterface_H

#include <vector>

#include <G4ThreeVector.hh>

#include "DDML/DDML.h"

class G4FastTrack;

namespace ddml {

/** The basic interface for the detector geometry - converting between global
 * (envelope) and local coordinates. The convention for the local coordinate
 * system is a right-handed coordinate system that has the z-axis pointing into
 *  the calorimeter, normal to the calorimeter planes.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

class GeometryInterface {
public:
  virtual ~GeometryInterface(){};

  /** compute local direction in coordinate system that has the z-axis pointing
   * into the calorimeter, normal to the layers
   */
  virtual G4ThreeVector localDirection(G4FastTrack const& aFastTrack) const = 0;

  /** convert the local spacepoints to global spacepoints inside sensitive
   * volumes
   */
  virtual void localToGlobal(G4FastTrack const& aFastTrack, std::vector<SpacePointVec>& spacepoints) const = 0;
};

} // namespace ddml

#endif
