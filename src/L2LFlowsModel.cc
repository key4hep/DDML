#include "DDML/L2LFlowsModel.h"

#include <G4FastTrack.hh>

#define DEBUGPRINT 0

namespace ddml {

void L2LFlowsModel::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir, InputVecs& inputs,
                                 TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = m_tensDims;

  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
  G4ThreeVector localDirNormed = localDir.unit();

  // resize input vectors according to the tensor dimensions
  inputs.resize(tensDims.size());
  for (std::size_t i = 0; i < tensDims.size(); i++) {
    int size = 1;
    for (std::size_t j = 0; j < tensDims[i].size(); j++) {
      size *= tensDims[i][j];
    }
    inputs[0].resize(size);
  }

  // For now, assume batch size one, and just assign values
  inputs[0][0] = energy / CLHEP::GeV;
  inputs[0][1] = -1. * localDirNormed.x();
  inputs[0][2] = -1. * localDirNormed.y();
  inputs[0][3] = localDirNormed.z();

  if (DEBUGPRINT) {
    std::cout << " e_I : " << inputs[0][0] << std::endl;
    std::cout << " p_x : " << inputs[0][1] << std::endl;
    std::cout << " p_y : " << inputs[0][2] << std::endl;
    std::cout << " p_z : " << inputs[0][3] << std::endl;
  }

  // ----  resize output vector
  int outputSize = m_nCellsX * m_nCellsY * m_nCellsZ;
  output.assign(outputSize, 0);
}

void L2LFlowsModel::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const& localDir,
                                  const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  std::vector<float> x_shift;
  std::vector<float> y_shift;
  _shift(localDir.unit(), x_shift, y_shift);

  spacepoints.resize(m_nCellsZ);

  for (int i = 0; i < m_nCellsZ; i++) {
    for (int j = 0; j < m_nCellsX; j++) {
      for (int k = 0; k < m_nCellsY; k++) {
        int idx = i * m_nCellsX * m_nCellsY + j * m_nCellsY + k;

        if (output[idx] > 1e-5) {
          ddml::SpacePoint sp(-1. * (j - m_nCellsX / 2. + 0.5) * m_cellSizeX + x_shift[i], // x
                              -1. * (k - m_nCellsY / 2. + 0.5) * m_cellSizeY + y_shift[i], // y
                              0.,                                                          // z
                              output[idx] * 1e3,                                           // energy
                              0.                                                           // time
          );

          spacepoints[i].push_back(sp);
        }
      }
    }
  }
}

void L2LFlowsModel::_shift(const G4ThreeVector& localDirNormed, std::vector<float>& x_shift,
                           std::vector<float>& y_shift) {
  x_shift.resize(m_nCellsZ);
  y_shift.resize(m_nCellsZ);
  for (int i = 0; i < m_nCellsZ; i++) {
    float dist_to_layers = m_layerBottomPos[i] - 1804.7 + m_cellThickness / 2.0;
    float r = dist_to_layers / localDirNormed.z();
    x_shift[i] = r * localDirNormed.x();
    y_shift[i] = r * localDirNormed.y();
  }
}

} // namespace ddml
