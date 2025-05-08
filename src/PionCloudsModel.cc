#include "DDML/PionCloudsModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#define DEBUGPRINT 0

namespace ddml {

  void PionCloudsModel::prepareInput(G4FastTrack const& aFastTrack,
			      G4ThreeVector const& localDir,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) {

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

  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "DDML::localDir: (%f, %f, %f)",
                   localDir_.x(), localDir_.y(), localDir_.z());

  // compute local incident angles
  double r = sqrt(localDir_.x() * localDir_.x() + localDir_.y() * localDir_.y() + localDir_.z() * localDir_.z());
  double theta = acos(localDir_.z() / r) / M_PI * 180.;
  double phi = atan2(localDir_.y(), localDir_.x()) / M_PI * 180.;

  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "DDML::localDir: (%f, %f)", theta, phi);
  // std::cout << "PionClouds::localDir:" << " theta = " << theta << " phi = " << phi << std::endl;

  // the input for the PionClouds is one energy and two angles (local Theta and Phi)
  inputs.resize(m_latentSize);

  inputs[0].resize(1); // Energy
  inputs[1].resize(1); // Theta
  inputs[2].resize(1); // Phi

  // For now, assume batch size one, and just assign values
  inputs[0][0] = energy / CLHEP::GeV; // E_vec[0]/100.;
  inputs[1][0] = theta;               // 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
  inputs[2][0] = phi;

  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_energy_tensor : %f", inputs[0][0]);
  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_theta_tensor : %f", inputs[1][0]);
  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_phi_tensor : %f", inputs[2][0]);

  // ----  resize output vector
  
  output.assign(m_maxNumElements, 0);
}


// For array structure: (No. showers, No. points, dimensions(4))
void PionCloudsModel::convertOutput(G4FastTrack const& aFastTrack,
  G4ThreeVector const& localDir,
  const std::vector<float>& output,
  std::vector<SpacePointVec>& spacepoints ){

    //int nPoints = m_numPoints ; // number of points in shower

    int layerNum = 0;

    std::vector<std::vector<float>> reshaped(m_numPoints, std::vector<float>(4));

    // Fill the 3D array-like vector using the flattened vector
    int index = 0;
      for (int j = 0; j < m_numPoints; ++j) {
          for (int k = 0; k < 4; ++k) {
              reshaped[j][k] = output[index];
              index++;
          }
      }

    spacepoints.resize( m_nLayer ) ;

    for (int i = 0; i < m_numPoints; i++) {
    ddml::SpacePoint sp(
    reshaped[i][0],   // x // *(-1) to align local to global convention in ddml
    reshaped[i][2],   // y // *(-1) to align local to global convention in ddml
    0.,             // z
    reshaped[i][3],   // energy
    0.              // time
    );
    layerNum = reshaped[i][1];
    spacepoints[layerNum].emplace_back( sp ) ;
    }
  }
}