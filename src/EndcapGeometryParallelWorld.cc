#include "DDML/EndcapGeometryParallelWorld.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Detector.h"
#include "DDRec/DetectorData.h"

#define DEBUGPRINT 0

namespace ddml {

void EndcapGeometryParallelWorld::initialize() {
  auto& theDetector = dd4hep::Detector::getInstance();
  auto det = theDetector.detector(_detector);
  auto* cal = det.extension<dd4hep::rec::LayeredCalorimeterData>();

  if (cal) {
    for (auto l : cal->layers) {
      _caloLayerDistances.push_back((l.distance + l.inner_thickness) / dd4hep::mm);
    }

  } else {
    std::cout << " ###### error:  detector " << _detector << " not found !" << std::endl;
  }
}

G4ThreeVector EndcapGeometryParallelWorld::localDirection(G4FastTrack const& aFastTrack) const {
  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // now transform this direction into a right handed coordinate system that has the z-axis pointing into the calo

  G4ThreeVector localDir(direction);

  // in the positive endcap the global is the correct local coordinate system
  //    on the other side it has to be rotated around the y-axis by pi
  if (position.z() < 0) {
    localDir = {-direction.x(), direction.y(), -direction.z()};
  }

  if (DEBUGPRINT) {
    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    std::cout << "  EndcapGeometryParallelWorld::localDirection "
              << " pos0 = " << position << " - dir = " << direction << " - E = "
              << " - localDir = " << localDir << energy << std::endl;
  }

  return localDir;
}

void EndcapGeometryParallelWorld::localToGlobal(G4FastTrack const& aFastTrack,
                                                std::vector<SpacePointVec>& spacepoints) const {
  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  if (DEBUGPRINT) {
    std::cout << "  EndcapGeometryParallelWorld::localToGlobal   pos0 = " << position << " - dir = " << direction
              << " - E = " << energy << std::endl;
  }

  float signZ = (position.z() > 0. ? 1.0 : -1.0);

  for (int l = 0; l < _nCellsZ; ++l) {
    for (int i = 0, N = spacepoints[l].size(); i < N; ++i) {
      auto& sp = spacepoints[l][i];

      // take the rotation by pi around y on the negative side into account
      sp.X = position.x() + signZ * sp.X;
      sp.Y += position.y();
      sp.Z = position.z() + signZ * sp.Z;
    }
  }
}
} // namespace ddml
