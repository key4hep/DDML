#include "DDML/L2LFlowsModel.h"

#include <G4FastTrack.hh>
#include <cstdlib>

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

  dd4hep::printout(dd4hep::DEBUG, "L2LFlowsModel::prepareInput", "Input_energy : %f", inputs[0][0]);
  dd4hep::printout(dd4hep::DEBUG, "L2LFlowsModel::prepareInput", "Input_p_x : %f", inputs[0][1]);
  dd4hep::printout(dd4hep::DEBUG, "L2LFlowsModel::prepareInput", "Input_p_y : %f", inputs[0][2]);
  dd4hep::printout(dd4hep::DEBUG, "L2LFlowsModel::prepareInput", "Input_p_z : %f", inputs[0][3]);

  // ----  resize output vector
  int outputSize = m_nCellsX * m_nCellsY * m_nCellsZ;
  output.assign(outputSize, 0);
}

void L2LFlowsModel::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const& localDir,
                                  const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  std::vector<float> x_shift;
  std::vector<float> y_shift;
  shift(localDir.unit(), x_shift, y_shift);

  spacepoints.resize(m_nCellsZ);

  for (int i = 0; i < m_nCellsZ; i++) {
    for (int j = 0; j < m_nCellsX; j++) {
      for (int k = 0; k < m_nCellsY; k++) {
        int idx = i * m_nCellsX * m_nCellsY + j * m_nCellsY + k;

        if (output[idx] > 1e-5) {
          float offset_x, offset_y;
          if (m_randomShift) {
            offset_x = ((float)std::rand()) / ((float)RAND_MAX);
            offset_y = ((float)std::rand()) / ((float)RAND_MAX);
          } else {
            offset_x = 0.5;
            offset_y = 0.5;
          }

          ddml::SpacePoint sp(
              -1. * (j - m_nCellsX / 2. + offset_x) * m_cellSizeX / m_factor + x_shift[i] + m_gridShiftX, // x
              -1. * (k - m_nCellsY / 2. + offset_y) * m_cellSizeY / m_factor + y_shift[i] + m_gridShiftY, // y
              0.,                                                                                         // z
              output[idx] * 1e3,                                                                          // energy
              0.                                                                                          // time
          );

          spacepoints[i].push_back(sp);
        }
      }
    }
  }
}

void L2LFlowsModel::shift(const G4ThreeVector& localDirNormed, std::vector<float>& x_shift,
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
