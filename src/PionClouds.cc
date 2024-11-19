#include "DDML/PionCloudsModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

//#include <torch/script.h>

#define DEBUGPRINT 0

namespace ddml {

  void PionCloudsModel::prepareInput(G4FastTrack const& aFastTrack,
			      G4ThreeVector const& localDir,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) {

    tensDims = _tensDims ;

    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();
    G4RotationMatrix rotZ ;
    rotZ.rotateZ( M_PI/2. );
    G4RotationMatrix rotX ;
    rotX.rotateX( M_PI/2. );
    // this convention is used for the local coordinates in the dataset (model was trained in this convention)
    // G4ThreeVector localDir_ = rotZ * (rotX * direction) ; 

    
    G4ThreeVector localDir_ = localDir ; 
    localDir_.setX( -1. * localDir_.x() ) ; // *(-1) to align local to global convention in ddml
    localDir_.setY( -1. * localDir_.y() ) ; // *(-1) to align local to global convention in ddml
    // std::cout << "CaloClouds::localDir:" << "(" << localDir_.x() << "," << localDir_.y() << "," << localDir_.z() << ")" << std::endl ;
    // std::cout << "DDML::localDir:" << "(" << localDir.x() << "," << localDir.y() << "," << localDir.z() << ")" << std::endl ;

    // compute local incident angles
    double r = sqrt( localDir_.x()*localDir_.x() + localDir_.y()*localDir_.y() + localDir_.z()*localDir_.z() ) ;
    double theta = acos( localDir_.z() / r ) / M_PI * 180. ;
    double phi = atan2( localDir_.y() , localDir_.x() ) / M_PI * 180.;

    // std::cout << "CaloClouds::localDir:" << " theta = " << theta << " phi = " << phi << std::endl ;

    // the input for the CaloCLouds is one energy and two angles (local Theta and Phi)
    inputs.resize(_latentSize);

    inputs[0].resize(1);   // Energy
    inputs[1].resize(1);   // Theta
    inputs[2].resize(1);   // Phi

    // For now, assume batch size one, and just assign values
    inputs[0][0] = energy / CLHEP::GeV;//E_vec[0]/100.;
    inputs[1][0] = theta; // 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
    inputs[2][0] = phi;
    

    if(DEBUGPRINT) {
      std::cout << " Input_energy_tensor : " <<   inputs[0][0] << std::endl ;
      std::cout << " Input_theta_tensor : " <<   inputs[1][0] << std::endl ;
      std::cout << " Input_phi_tensor : " <<   inputs[2][0] << std::endl ;
    } 

    // ----  resize output vector

    int outputSize = _maxNumElements ;

    output.assign(outputSize, 0);
  }

  
  void PionCloudsModel::convertOutput(G4FastTrack const& aFastTrack,
              G4ThreeVector const& localDir,
              const std::vector<float>& output,
              std::vector<SpacePointVec>& spacepoints ){
  
    int nPoints = _numPoints ; // number of points in shower

    spacepoints.resize( 78 ) ;

    for (int i = 0; i < nPoints; i++) {
        ddml::SpacePoint sp(
            output[i][0],   // x // *(-1) to align local to global convention in ddml
            output[i][2],   // y // *(-1) to align local to global convention in ddml
            0.,             // z
            output[i][3],   // energy
            0.              // time
        );
        layerNum = output[i][1];
	    spacepoints[layerNum].emplace_back( sp ) ;
    }
  }
}
