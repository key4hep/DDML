#ifndef GeometryInterface_H
#define GeometryInterface_H

#include <concepts>
#include <vector>

#include <G4ThreeVector.hh>

#include "DDML/DDML.h"

namespace dd4hep::sim {
class Geant4Action;
}

class G4FastTrack;

namespace ddml {

/** The basic concept for the detector geometry - converting between global
 * (envelope) and local coordinates. The convention for the local coordinate
 * system is a right-handed coordinate system that has the z-axis pointing into
 *  the calorimeter, normal to the calorimeter planes.
 *
 *  @author F.Gaede, DESY
 *  @date Mar 2023
 */

template <typename T>
concept GeometryInterface = requires(const T t, T mt, const G4FastTrack& aFastTrack,
                                     std::vector<SpacePointVec>& spacepoints, dd4hep::sim::Geant4Action* plugin) {
  /** compute local direction in coordinate system that has the z-axis pointing
   * into the calorimeter, normal to the layers
   */
  { t.localDirection(aFastTrack) } -> std::same_as<G4ThreeVector>;

  /** convert the local spacepoints to global spacepoints inside sensitive
   * volumes
   */
  { t.localToGlobal(aFastTrack, spacepoints) } -> std::same_as<void>;

  /// declareProperties will be called from the FastMLShower constructor
  { mt.declareProperties(plugin) } -> std::same_as<void>;
};

} // namespace ddml

#endif
