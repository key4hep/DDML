#include "DDML/Par04ExampleVAE.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "CLHEP/Random/RandFlat.h"  // for RandGauss
#include "CLHEP/Random/RandGauss.h" // for RandGauss

#define DEBUGPRINT 0

namespace ddml {

void Par04ExampleVAE::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                                   TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = m_tensDims;

  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // compute local incident angle
  double theta = acos(localDir.z());

  if (DEBUGPRINT) {
    std::cout << "  Par04ExampleVAE::prepareInput   pos0 = " << position << " - dir = " << direction
              << " - E = " << energy / CLHEP::GeV << " theta = " << theta * 180. / M_PI << std::endl;
  }

  // the input for this model is the latent space and the energy conditioning

  if (inputs.size() != 1) {
    inputs.resize(1);
  }

  inputs[0].resize(m_latentSize + 4);

  for (int i = 0; i < m_latentSize; ++i) {
    inputs[0][i] = CLHEP::RandGauss::shoot(0., 1.);
  }

  /// Maximum particle energy value (in MeV) in the training range
  float fMaxEnergy = 1024000.0;
  /// Maximum particle angle (in degrees) in the training range
  float fMaxAngle = M_PI / 2.; // 90.0 deg ;

  inputs[0][m_latentSize] = (energy / CLHEP::MeV) / fMaxEnergy;
  inputs[0][m_latentSize + 1] = theta / fMaxAngle;
  inputs[0][m_latentSize + 2] = 0;
  inputs[0][m_latentSize + 3] = 1;

  m_initialEnergy = energy / CLHEP::MeV;

  // ----  resize output vector
  int outputSize = m_nCellsRho * m_nCellsPhi * m_nCellsZ;

  output.assign(outputSize, 0);
}

void Par04ExampleVAE::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const&,
                                    const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  int nLayer = m_nCellsZ; // number of layers is z dimension

  spacepoints.resize(nLayer);

  for (int l = 0; l < nLayer; ++l) {
    spacepoints[l].reserve(m_nCellsRho * m_nCellsPhi);
  }

  int iHit = 0;

  for (int i = 0; i < m_nCellsRho; ++i) {
    float rho = (i + 0.5) * m_cellSizeRho;

    for (int j = 0; j < m_nCellsPhi; ++j) {
      float phiCell = (j + 0.5) * 2. * CLHEP::pi / m_nCellsPhi;
      float x = rho * cos(phiCell);
      float y = rho * sin(phiCell);

      for (int l = 0; l < nLayer; ++l) {
        if (output[iHit] > 0.) {
          ddml::SpacePoint sp(x, y, 0.,
                              output[iHit] * m_initialEnergy, // model output is in fraction of initial E
                              0.);

          spacepoints[l].emplace_back(sp);
        }

        ++iHit;
      }
    }
  }
}
} // namespace ddml
