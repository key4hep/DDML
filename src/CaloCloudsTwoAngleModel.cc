#include "DDML/CaloCloudsTwoAngleModel.h"

#include <G4FastTrack.hh> // for G4FastTrack

// #include <torch/script.h>

#define DEBUGPRINT 0

namespace ddml {

void CaloCloudsTwoAngleModel::prepareInput(G4FastTrack const& aFastTrack, G4ThreeVector const& localDir,
                                           InputVecs& inputs, TensorDimVecs& tensDims, std::vector<float>& output) {
  tensDims = m_tensDims;

  G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

  G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();
  G4RotationMatrix rotZ;
  rotZ.rotateZ(M_PI / 2.);
  G4RotationMatrix rotX;
  rotX.rotateX(M_PI / 2.);
  // this convention is used for the local coordinates in the dataset (model was trained in this convention)
  // G4ThreeVector localDir_ = rotZ * (rotX * direction) ;

  G4ThreeVector localDir_ = localDir;
  localDir_.setX(-1. * localDir_.x()); // *(-1) to align local to global convention in ddml
  localDir_.setY(-1. * localDir_.y()); // *(-1) to align local to global convention in ddml

  dd4hep::printout(dd4hep::DEBUG, "CaloCloudsTwoAngleModel::prepareInput", "DDML::localDir: (%f, %f, %f)",
                   localDir_.x(), localDir_.y(), localDir_.z());
  // std::cout << "CaloClouds::localDir:" << "(" << localDir_.x() << "," << localDir_.y() << "," << localDir_.z() << ")"
  //           << std::endl;

  // compute local incident angles
  double r = sqrt(localDir_.x() * localDir_.x() + localDir_.y() * localDir_.y() + localDir_.z() * localDir_.z());
  double theta = acos(localDir_.z() / r) / M_PI * 180.;
  double phi = atan2(localDir_.y(), localDir_.x()) / M_PI * 180.;

  dd4hep::printout(dd4hep::DEBUG, "CaloCloudsTwoAngleModel::prepareInput", "DDML::localDir: (%f, %f)", theta, phi);
  // std::cout << "CaloClouds::localDir:" << " theta = " << theta << " phi = " << phi << std::endl;

  // the input for the CaloCLouds is one energy and two angles (local Theta and Phi)
  inputs.resize(m_latentSize);

  inputs[0].resize(1); // Energy
  inputs[1].resize(1); // Theta
  inputs[2].resize(1); // Phi

  // For now, assume batch size one, and just assign values
  inputs[0][0] = energy / CLHEP::GeV; // E_vec[0]/100.;
  inputs[1][0] = theta;               // 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
  inputs[2][0] = phi;

  // if (DEBUGPRINT) {
  //   std::cout << " Input_energy_tensor : " << inputs[0][0] << std::endl;
  //   std::cout << " Input_theta_tensor : " << inputs[1][0] << std::endl;
  //   std::cout << " Input_phi_tensor : " << inputs[2][0] << std::endl;
  // }

  dd4hep::printout(dd4hep::DEBUG, "CaloCloudsTwoAngleModel::prepareInput", "Input_energy_tensor : %f", inputs[0][0]);
  dd4hep::printout(dd4hep::DEBUG, "CaloCloudsTwoAngleModel::prepareInput", "Input_theta_tensor : %f", inputs[1][0]);
  dd4hep::printout(dd4hep::DEBUG, "CaloCloudsTwoAngleModel::prepareInput", "Input_phi_tensor : %f", inputs[2][0]);

  // ----  resize output vector

  output.assign(m_maxNumElements, 0);
}

void CaloCloudsTwoAngleModel::convertOutput(G4FastTrack const&, G4ThreeVector const&, const std::vector<float>& output,
                                            std::vector<SpacePointVec>& spacepoints) {
  int nLayer = m_nLayer; // number of layers is z dimension
  int layerNum = 0;
  int numSP = 0;

  spacepoints.resize(nLayer);

  int numElements = 0;
  for (int i = 0; i < nLayer; i++) {
    numSP = output[i] + 1;
    spacepoints[i].reserve(numSP);
    numElements += output[i] * 4;
  }

  for (int i = nLayer; i < nLayer + numElements; i += 4) {
    ddml::SpacePoint sp(output[i] * (-1.),    // x // *(-1) to align local to global convention in ddml
                        output[i + 1] * (-1), // y // *(-1) to align local to global convention in ddml
                        0.,                   // z
                        output[i + 3],        // energy
                        0.                    // time
    );

    // layerNum = output[i+2] ;
    layerNum = output[i + 2];

    spacepoints[layerNum].emplace_back(sp);
  }
}

} // namespace ddml
