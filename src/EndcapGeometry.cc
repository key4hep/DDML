#include "DDML/EndcapGeometry.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Detector.h"
#include "DDRec/DetectorData.h"

#define DEBUGPRINT 0

namespace ddml {

void EndcapGeometry::initialize() {
  auto& theDetector = dd4hep::Detector::getInstance();
  auto det = theDetector.detector(m_detector);
  auto* cal = det.extension<dd4hep::rec::LayeredCalorimeterData>();

  if (cal) {
    for (auto l : cal->layers) {
      m_caloLayerDistances.push_back((l.distance + l.inner_thickness) / dd4hep::mm);
    }

  } else {
    std::cout << " ###### error:  detector " << m_detector << " not found !" << std::endl;
  }
}

G4ThreeVector EndcapGeometry::localDirection(G4FastTrack const& aFastTrack) const {
  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // now transform this direction into a right handed coordinate system that has
  // the z-axis pointing into the calo

  G4ThreeVector localDir(direction);

  // in the positive endcap the global is the correct local coordinate system
  //    on the other side it has to be rotated around the y-axis by pi
  if (position.z() < 0) {
    localDir = {-direction.x(), direction.y(), -direction.z()};
  }

  if (DEBUGPRINT) {
    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    std::cout << "  EndcapGeometry::localDirection "
              << " pos0 = " << position << " - dir = " << direction << " - E = "
              << " - localDir = " << localDir << energy << std::endl;
  }

  return localDir;
}

void EndcapGeometry::localToGlobal(G4FastTrack const& aFastTrack, std::vector<SpacePointVec>& spacepoints) const {
  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  if (DEBUGPRINT) {
    std::cout << "  EndcapGeometry::localToGlobal   pos0 = " << position << " - dir = " << direction
              << " - E = " << energy << std::endl;
  }

  float signZ = (position.z() > 0. ? 1.0 : -1.0);

  if (!m_correctForAngles) {
    direction = {0., 0., signZ * 1.0}; // position layers w/ impact normal to the plane
  }

  // find the first layer that will have signals as sometimes particles are
  // create in the calorimeter !
  int firstLayer = 0;
  int nLayer = m_caloLayerDistances.size();

  for (int l = 0; l < nLayer; ++l) {
    double zL = signZ * m_caloLayerDistances[l];
    firstLayer = l;
    // lamda for intersection of particle direction and calo plane in phi sector
    // 0
    double lambda = (zL - position.z()) / direction.z();
    if (lambda > 0.) {
      break;
    }
  }

  nLayer -= firstLayer; // only populate existing calo layers

  // and remove other layers
  for (unsigned k = nLayer; k < spacepoints.size(); ++k) {
    spacepoints[k].clear();
  }

  for (int l = 0; l < nLayer; ++l) {
    double zL = signZ * m_caloLayerDistances[l + firstLayer];

    // lamda for intersection of particle direction and calo plane in phi sector
    // 0
    double lambda = (zL - position.z()) / direction.z();

    G4ThreeVector posC = position + lambda * direction;

    for (int i = 0, N = spacepoints[l].size(); i < N; ++i) {
      auto& sp = spacepoints[l][i];

      // take the rotation by pi around y on the negative side into account
      sp.X = posC.x() + signZ * sp.X;
      sp.Y += posC.y();
      sp.Z = zL; // =  posC.z()
    }
  }
}
} // namespace ddml
