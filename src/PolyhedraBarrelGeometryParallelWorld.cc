#include "DDML/PolyhedraBarrelGeometryParallelWorld.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Detector.h"
#include "DDRec/DetectorData.h"

#define DEBUGPRINT 1

namespace ddml {

void PolyhedraBarrelGeometryParallelWorld::initialize() {
  
  /************* For Debugging!!!! ***********/
  auto& theDetector = dd4hep::Detector::getInstance();
  auto det = theDetector.detector(m_detector);
  auto* cal = det.extension<dd4hep::rec::LayeredCalorimeterData>();

  if (cal) {
    for (auto l : cal->layers) {
      m_caloLayerDistances.push_back((l.distance + l.inner_thickness) / dd4hep::mm);
      std::cout << " ECAL Layer distances " << l.distance + l.inner_thickness << std::endl;
    }
  } else {
    std::cout << " ###### error:  detector " << m_detector << " not found !" << std::endl;
  }

  /************ End of for Debugging!!! *********/

  std::cout << " PolyhedraBarrelGeometryParallelWorld: detector is " << m_detector << std::endl;
}

int PolyhedraBarrelGeometryParallelWorld::phiSector(G4ThreeVector const& position) const {
  // compute phi sector, e.g. 0-7 for octagonal barrel
  //         2
  //        --
  //    3 /    \ 1
  //   4 |      | 0
  //    5 \    / 7
  //        --
  //        6

  double phi = position.phi();
  if (phi < 0.) {
    phi = 2. * M_PI + phi;
  }

  int phiSec = phi / (M_PI / _nSymmetry);

  if (phiSec == 0 || phiSec == (2 * _nSymmetry - 1)) {
    phiSec = 0;
  } else {
    phiSec = (phiSec + 1) / 2.;
  }

  return phiSec;
}

G4ThreeVector PolyhedraBarrelGeometryParallelWorld::localDirection(G4FastTrack const& aFastTrack) const {
  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  int phiSec = phiSector(position);

  // --- rotate position and direction to phi sector 0  (calo plane parallel to y-axis at positive x )

  G4RotationMatrix rotNeg;
  rotNeg.rotateZ(-phiSec * 2. * M_PI / _nSymmetry);

  // new direction in global coordinates
  auto dirR = rotNeg * direction;

  // now transform this direction into a right handed coordinate system that has the z-axis pointing into the calo
  G4ThreeVector localDir(-dirR.z(), dirR.y(), dirR.x());

  if (DEBUGPRINT) {
    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    std::cout << "  PolyhedraBarrelGeometryParallelWorld::localDirection  - symmetry = " << _nSymmetry
              << " pos0 = " << position << " - dir = " << direction << " - E = "
              << " - localDir = " << localDir << energy << std::endl;
    std::cout << "  PolyhedraBarrelGeometryParallelWorld::localDirection  - phi = "
              << atan2(localDir.y(), localDir.x()) / M_PI * 180. << "   theta : " << acos(localDir.z()) / M_PI * 180.
              << std::endl;
  }

  return localDir;
}

void PolyhedraBarrelGeometryParallelWorld::localToGlobal(G4FastTrack const& aFastTrack,
                                                         std::vector<SpacePointVec>& spacepoints) const {
  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  int phiSec = phiSector(position);

  if (DEBUGPRINT) {
    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    std::cout << "  PolyhedraBarrelGeometryParallelWorld::localToGlobal  - symmetry = " << _nSymmetry
              << " pos0 = " << position << " - dir = " << direction << " - E = " << energy << std::endl;
  }

  // --- rotate position and direction to phi sector 0  (calo plane parallel to y-axis at positive x )

  G4RotationMatrix rotNeg;
  rotNeg.rotateZ(-phiSec * 2. * M_PI / _nSymmetry);

  G4RotationMatrix rotPos;
  rotPos.rotateZ(+phiSec * 2. * M_PI / _nSymmetry);

  auto posR = rotNeg * position;
  auto dirR = rotNeg * direction;

  if (!_correctForAngles) {
    dirR = {1., 0., 0.}; // position layers w/ impact normal to the plane
  }

  if (DEBUGPRINT) {
    std::cout << "  PolyhedraBarrelGeometryParallelWorld::localToGlobal -  position " << position << " - direction "
              << direction << " phiSec: " << phiSec << " posR " << posR << " dirR " << dirR << std::endl;
  }

  for (int l = 0; l < _nCellsZ; ++l) {
    for (int i = 0, N = spacepoints[l].size(); i < N; ++i) {
      auto& sp = spacepoints[l][i];

      /// PARALLEL WORLD LOCAL TO GLOBAL
      // In this case, don''t use any information about the sensitive positions,
      // but rather only about the position of the envelope (i.e. surface of the calorimeter face)
      G4ThreeVector pos(posR.x() + sp.Z, posR.y() + sp.Y, posR.z() - sp.X);

      // rotate back to original phi sector
      auto global = rotPos * pos;

      sp.X = global.x();
      sp.Y = global.y();
      sp.Z = global.z();

      std::cout << "PolyhedraBarrelGeometryParallelWorld::localToGlobal --- sp.X: " << sp.X << ", sp.Y: " << sp.Y << ", sp.Z: " << sp.Z << std::endl; 
    }
  }
}
} // namespace ddml
