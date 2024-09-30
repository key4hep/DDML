#include "DDML/RegularGridBIBAEModel.h"

#include <G4FastTrack.hh> // for G4FastTrack

// #include <torch/script.h>

#define DEBUGPRINT 1

namespace ddml {

void RegularGridBIBAEModel::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir,
                                         InputVecs& inputs, TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = m_tensDims;

  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // compute local incident angle
  double theta = acos(localDir.z());

  if (DEBUGPRINT) {
    std::cout << "  RegularGridBIBAEModel::prepareInput   pos0 = " << position << " - dir = " << direction
              << " - E = " << energy / CLHEP::GeV << " theta = " << theta << std::endl;
  }

  // the input for the BIB-AE is one energy and an angle (plus cond tensor)
  inputs.resize(m_latentSize);

  inputs[0].resize(1);
  inputs[1].resize(1);
  inputs[2].resize(2);

  // For now, assume batch size one, and just assign values
  inputs[0][0] = energy / CLHEP::GeV; // E_vec[0]/100.;
  inputs[1][0] = M_PI / 2. - theta;   // 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
  inputs[2][0] = (inputs[0][0]) / 100.;
  inputs[2][1] = (inputs[1][0]) / (90. * (M_PI / 180.));

  if (DEBUGPRINT) {
    std::cout << " Input_energy_tensor : " << inputs[0][0] * 100. << std::endl;
  }

  // ----  resize output vector

  int outputSize = m_nCellsX * m_nCellsY * m_nCellsZ;

  output.assign(outputSize, 0);
}

void RegularGridBIBAEModel::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const& localDir,
                                          const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  // compute local incident anngles

  double phi = atan2(localDir.y(), localDir.x());

  int nLayer = m_nCellsZ; // number of layers is z dimension

  spacepoints.resize(nLayer);

  int iHit = 0;

  for (int l = 0; l < nLayer; ++l) {
    spacepoints[l].reserve(m_nCellsX * m_nCellsY);

    for (int i = 0; i < m_nCellsX; ++i) {
      for (int j = 0; j < m_nCellsY; ++j) {
        if (output[iHit] > 0.) {
          // in the current BIB-AE x and y are switched, i.e. the angle is
          // changed along y
          double y = (i - int(m_centerCellX) + 0.5) * m_cellSizeX;
          double x = (j - int(m_centerCellY) + 0.5) * m_cellSizeY;

          // rotate the individual layers corresponding to the local azimuth
          // angle phi
          ddml::SpacePoint sp(x * cos(phi) - y * sin(phi), x * sin(phi) + y * cos(phi), 0., output[iHit], 0.);

          spacepoints[l].emplace_back(sp);
        }

        ++iHit;
      }
    }
  }
}
} // namespace ddml
