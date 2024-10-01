#include "DDML/RegularGridGANModel.h"

#include <G4FastTrack.hh> // for G4FastTrack

#include "CLHEP/Random/RandFlat.h"  // for RandGauss
#include "CLHEP/Random/RandGauss.h" // for RandGauss

#define DEBUGPRINT 1

namespace ddml {

void RegularGridGANModel::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const&, InputVecs& inputs,
                                       TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = m_tensDims;

  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector position = aFastTrack.GetPrimaryTrack()->GetPosition();
  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

  // here we could use position and direction to compute additional
  // conditioning variables, such as incident angles ...
  // for now assume simple GAN with 90 deg incident

  if (DEBUGPRINT) {
    std::cout << "  RegularGridGANModel::prepareInput   pos0 = " << position << " - dir = " << direction
              << " - E = " << energy / CLHEP::GeV << std::endl;
  }

  // the input for this model is the latent space and the energy conditioning

  if (inputs.size() != 2) {
    inputs.resize(2);
  }

  inputs[0].resize(m_latentSize);
  inputs[1].resize(1);

  for (int i = 0; i < m_latentSize; ++i) {
    inputs[0][i] = CLHEP::RandFlat::shoot(-1., 1.);

    //    genVector[i] = CLHEP::RandGauss::shoot(0., 1.);
  }

  inputs[1][0] = energy / CLHEP::GeV;

  // ... later ...
  // input[ _latentSize + 1 ] = angle ;

  // ----  resize output vector
  int outputSize = m_nCellsX * m_nCellsY * m_nCellsZ;

  output.assign(outputSize, 0);
}

void RegularGridGANModel::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const&,
                                        const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  int nLayer = m_nCellsZ; // number of layers is z dimension

  spacepoints.resize(nLayer);

  int iHit = 0;

  for (int l = 0; l < nLayer; ++l) {
    spacepoints[l].reserve(m_nCellsX * m_nCellsY);

    for (int i = 0; i < m_nCellsX; ++i) {
      for (int j = 0; j < m_nCellsY; ++j) {
        if (output[iHit] > 0.) {
          ddml::SpacePoint sp((i - int(m_nCellsX / 2) + 0.5) * m_cellSizeX,
                              (j - int(m_nCellsY / 2) + 0.5) * m_cellSizeY, 0., output[iHit], 0.);

          spacepoints[l].emplace_back(sp);
        }

        ++iHit;
      }
    }
  }
}
} // namespace ddml
