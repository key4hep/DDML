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
  int outputSize = m_nBinsX * m_nBinsY * m_nBinsZ;
  output.assign(outputSize, 0);
}

void L2LFlowsModel::convertOutput(G4FastTrack const& /*aFastTrack*/, G4ThreeVector const& /*localDir*/,
                                  const std::vector<float>& output, std::vector<SpacePointVec>& spacepoints) {
  spacepoints.resize(m_nBinsZ);

  for (int i = 0; i < m_nBinsZ; i++) {
    for (int j = 0; j < m_nBinsX; j++) {
      for (int k = 0; k < m_nBinsY; k++) {
        int idx = i * m_nBinsX * m_nBinsY + j * m_nBinsY + k;

        if (output[idx] > 1e-5) {
          float offset_x, offset_y;
          if (m_randomShift) {
            offset_x = ((float)std::rand()) / ((float)RAND_MAX);
            offset_y = ((float)std::rand()) / ((float)RAND_MAX);
          } else {
            offset_x = 0.5;
            offset_y = 0.5;
          }

          ddml::SpacePoint sp(-1. * (j - m_nBinsX / 2. + offset_x) * m_cellSizeX / m_factor + m_gridShiftX, // x
                              -1. * (k - m_nBinsY / 2. + offset_y) * m_cellSizeY / m_factor + m_gridShiftY, // y
                              0.,                                                                           // z
                              output[idx] * CLHEP::GeV,                                                     // energy
                              0.                                                                            // time
          );

          spacepoints[i].push_back(sp);
        }
      }
    }
  }
}

} // namespace ddml
