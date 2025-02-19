#include "DDML/PionCloudsModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

//#include <torch/script.h>

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
  // std::cout << "PionClouds::localDir:" << "(" << localDir_.x() << "," << localDir_.y() << "," << localDir_.z() << ")"
  //           << std::endl;

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

  // if (DEBUGPRINT) {
  //   std::cout << " Input_energy_tensor : " << inputs[0][0] << std::endl;
  //   std::cout << " Input_theta_tensor : " << inputs[1][0] << std::endl;
  //   std::cout << " Input_phi_tensor : " << inputs[2][0] << std::endl;
  // }

  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_energy_tensor : %f", inputs[0][0]);
  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_theta_tensor : %f", inputs[1][0]);
  dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::prepareInput", "Input_phi_tensor : %f", inputs[2][0]);

  // ----  resize output vector
  
  output.assign(m_maxNumElements, 0);
}


  
  void PionCloudsModel::convertOutput(G4FastTrack const& aFastTrack,
              G4ThreeVector const& localDir,
              const std::vector<float>& output,
              std::vector<SpacePointVec>& spacepoints ){
  
    int nPoints = m_numPoints ; // number of points in shower

    dd4hep::printout(dd4hep::DEBUG, "PionCloudsModel::convertOutput", "m_numPoints : %i", m_numPoints);

    spacepoints.resize( m_nLayer ) ;

    int numSP = 0;
    int layerNum = 0;
    int numElements = 0;
    for (int i = 0; i < m_nLayer; i++) {
      numSP = output[i] + 1;
      spacepoints[i].reserve(numSP);
      numElements += output[i] * 4;
    }

    std::cout << " PionCloudsModel::convertOutput DONE numElements, numElements = " << numElements << std::endl;

    /*
    for (int i = m_nLayer; i < m_nLayer + numElements; i += 4) {
      std::cout << " PionCloudsModel::convertOutput Layer Loop, i = " << i << std::endl;
      std::cout << " PionCloudsModel::convertOutput Layer Loop, output[i] " << output[i] << std::endl;
      std::cout << " PionCloudsModel::convertOutput Layer Loop, output[i+1] " << output[i] << std::endl;
      std::cout << " PionCloudsModel::convertOutput Layer Loop, output[i+3] " << output[i] << std::endl;
          ddml::SpacePoint sp(
            output[i],   // x // *(-1) to align local to global convention in ddml
            output[i+1],   // y // *(-1) to align local to global convention in ddml
            0.,             // z
            output[i+3],   // energy
            0.              // time
        );
        layerNum = output[i+2];
        std::cout << " PionCloudsModel::convertOutput Layer Loop, layerNum " << layerNum << std::endl;
	    spacepoints[layerNum].emplace_back( sp ) ;
    */


    float reshaped[2600][4];

    // Fill the 3D array using the flattened vector
    size_t index = 0;
      for (size_t j = 0; j < 2600; ++j) {
          for (size_t k = 0; k < 4; ++k) {
              reshaped[j][k] = output[index++];
          }
      }

    for (int i = 0; i < nPoints; i++) {
        ddml::SpacePoint sp(
            reshaped[i][0],   // x // *(-1) to align local to global convention in ddml
            reshaped[i][2],   // y // *(-1) to align local to global convention in ddml
            0.,             // z
            reshaped[i][3],   // energy
            0.              // time
        );
        layerNum = reshaped[i][1];
        std::cout << "PionCloudsModel::convertOutput - layerNum" << layerNum <<std::endl;
        std::cout << "PionCloudsModel::convertOutput - x: " << reshaped[i][0]*1e3 << std::endl;
        std::cout << "PionCloudsModel::convertOutput - z: " << reshaped[i][2]*1e3 << std::endl;
	    spacepoints[layerNum].emplace_back( sp ) ;
    }

    std::cout << " PionCloudsModel::convertOutput FINISHED " << std::endl;
  }
}
