#include "DDML/Par04CylindrialScoringMeshVAE.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "CLHEP/Random/RandGauss.h"     // for RandGauss
#include "CLHEP/Random/RandFlat.h"     // for RandGauss

#define DEBUGPRINT 0

namespace ddml {



  void Par04CylindrialScoringMeshVAE::prepareInput(G4FastTrack const& aFastTrack,
				     G4ThreeVector const& localDir,
				     InputVecs& inputs, TensorDimVecs& tensDims,
				     std::vector<float>& output ) {

    tensDims = _tensDims ;

    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    
    // compute local incident angle
    double theta = acos( localDir.z() ) ;

    if( DEBUGPRINT ) 
      std::cout << "  Par04CylindrialScoringMeshVAE::prepareInput   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy / CLHEP::GeV
		<< " theta = " << theta * 180. / M_PI
		<< std::endl ;
    
    // the input for this model is the latent space and the energy conditioning
    
    if( inputs.size() != 1 )
      inputs.resize(1) ;
 
    inputs[0].resize( _latentSize + 4 );

    for(int i = 0; i < _latentSize; ++i)
    {
      inputs[0][i] =  CLHEP::RandGauss::shoot(0., 1.);
    }

    /// Maximum particle energy value (in MeV) in the training range
    float fMaxEnergy = 1024000.0;
    /// Maximum particle angle (in degrees) in the training range
    float fMaxAngle = M_PI / 2. ; //90.0 deg ;
    
    inputs[0][_latentSize ]     = (energy /CLHEP::MeV) / fMaxEnergy ;
    inputs[0][_latentSize + 1 ] = theta / fMaxAngle ;
    inputs[0][_latentSize + 2 ] = 0 ;
    inputs[0][_latentSize + 3 ] = 1 ;

    _initialEnergy = energy /CLHEP::MeV ;
    
    // ----  resize output vector
    int outputSize = _nCellsRho * _nCellsPhi * _nCellsZ ;
    
    output.assign(outputSize, 0);
  }
  
 // ORIGINAL IMPLEMENTATION 
/*
void Par04CylindrialScoringMeshVAE::convertOutput(G4FastTrack const& aFastTrack,
            G4ThreeVector const& localDir,
            const std::vector<float>& output,
            std::vector<SpacePointVec>& spacepoints ){

      // Information about primary particle from track
      //G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
      //G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();


      G4ThreeVector fMeshNumber = G4ThreeVector(_nCellsRho, _nCellsPhi, _nCellsZ);

      
      spacepoints.resize(fMeshNumber.x() * fMeshNumber.y() * fMeshNumber.z());


      // Calculate rotation matrix along the particle momentum direction
      // It will rotate the shower axes to match the incoming particle direction
      // This currently does the positioning of showers in the global coordinate position.
      // Should factorise out the global positioning to a local to global in the geo implementation.
      
      // compute local incident angle
      double theta = acos( localDir.z() ) ;
      double phi = atan2( localDir.y() , localDir.x() ) ;

      // Move to local to global!
      G4RotationMatrix rotMatrix = G4RotationMatrix();
      double particleTheta = theta;//direction.theta();
      double particlePhi = phi;//direction.phi();
      //rotMatrix.rotateZ(-particlePhi);
      //rotMatrix.rotateY(-particleTheta);

      rotMatrix.rotateZ(particlePhi);
      rotMatrix.rotateX(particleTheta);
      
      G4RotationMatrix rotMatrixInv = CLHEP::inverseOf(rotMatrix);

      */

      /*
      int cpt = 0;
      for (G4int iCellR = 0; iCellR < fMeshNumber.x(); iCellR++)
      {
        for (G4int iCellPhi = 0; iCellPhi < fMeshNumber.y(); iCellPhi++)
        {
          for (G4int iCellZ = 0; iCellZ < fMeshNumber.z(); iCellZ++)
          {
            spacepoints[cpt] =
                position +
                rotMatrixInv *
                    G4ThreeVector((iCellR + 0.5) * fMeshSize.x() *
                                  std::cos((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi),
                              (iCellR + 0.5) * fMeshSize.x() *
                                  std::sin((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi),
                              (iCellZ + 0.5) * fMeshSize.z());
            cpt++;
          }
        }
      }
      */

     /*
      // Attempt at local definition
      // Local conversion of model output to space points.
      int iHit = 0;
      for (G4int iCellR = 0; iCellR < fMeshNumber.x(); iCellR++)
      {
        for (G4int iCellPhi = 0; iCellPhi < fMeshNumber.y(); iCellPhi++)
        {
          for (G4int iCellZ = 0; iCellZ < fMeshNumber.z(); iCellZ++)
          {
              
                  x = (iCellR + 0.5) * fMeshSize.x() *
                                  std::cos((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi);
                  y = (iCellR + 0.5) * fMeshSize.x() *
                                  std::sin((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi);
                  z = (iCellZ + 0.5) * fMeshSize.z();

            ddml::SpacePoint sp(
            x,
            y,
            z,
            output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
            0.
            ) ;
          
              spacepoints[iHit].emplace_back( sp ) ;
            }

            iHit++;
          }
        }
      }
    
      ddml::SpacePoint sp(
	      x,
	      y,
	      0.,
	      output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  */

  
   // For using local coordinate convention of DDML, with addtional sensitive asignment
   // based on closest active layer
    // Local conversion of model output to space points.
    /*
    int iHit = 0;
    for (G4int iCellR = 0; iCellR < fMeshNumber.x(); iCellR++)
    {
      for (G4int iCellPhi = 0; iCellPhi < fMeshNumber.y(); iCellPhi++)
      {
        for (G4int iCellZ = 0; iCellZ < fMeshNumber.z(); iCellZ++)
        { 

               G4ThreeVector local_cylindrical_spacepoint = rotMatrixInv *
                    G4ThreeVector((iCellR + 0.5) * fMeshSize.x() *
                                  std::cos((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi),
                              (iCellR + 0.5) * fMeshSize.x() *
                                  std::sin((iCellPhi + 0.5) * 2 * CLHEP::pi /
                                               fMeshNumber.y() -
                                           CLHEP::pi),
                              (iCellZ + 0.5) * fMeshSize.z());
               
                //x = (iCellR + 0.5) * fMeshSize.x() *
                 //               std::cos((iCellPhi + 0.5) * 2 * CLHEP::pi /
                 //                             fMeshNumber.y() -
                 //                         CLHEP::pi);
                //y = (iCellR + 0.5) * fMeshSize.x() *
                 //               std::sin((iCellPhi + 0.5) * 2 * CLHEP::pi /
                 //                             fMeshNumber.y() -
                 //                         CLHEP::pi);
                //z = (iCellZ + 0.5) * fMeshSize.z();

                //double x = local_cylindrical_spacepoint.x();
                //double y = local_cylindrical_spacepoint.y();
                //double z = local_cylindrical_spacepoint.z();

                double x = local_cylindrical_spacepoint[0];
                double y = local_cylindrical_spacepoint[1];
                double z = local_cylindrical_spacepoint[2];

                if( output[ iHit ] > 0. ){ 

                    ddml::SpacePoint sp(
                    x,
                    y,
                    z,
                    output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
                    0.
                    ) ;
                  
                      spacepoints[iHit].emplace_back( sp ) ;
                }

          }
        }
      }
      iHit++;
    }
  */
    
void Par04CylindrialScoringMeshVAE::convertOutput(G4FastTrack const& aFastTrack,
            G4ThreeVector const& localDir,
            const std::vector<float>& output,
            std::vector<SpacePointVec>& spacepoints ){
  
    int nLayer = _nCellsZ ; // number of layers is z dimension
    
    spacepoints.resize( nLayer ) ; 

    for(int l=0 ; l < nLayer ; ++l){
      spacepoints[l].reserve( _nCellsRho * _nCellsPhi ) ;
    }

    int iHit = 0 ;

    for(int i=0; i<_nCellsRho; ++i){
      
      float rho = (i+0.5) * _cellSizeRho ;

      for(int j=0; j<_nCellsPhi; ++j){

	float phiCell = (j+0.5) * 2.*CLHEP::pi / _nCellsPhi ;
	float x = rho * cos( phiCell ) ;
	float y = rho * sin( phiCell ) ;
	

	for(int l=0 ; l < nLayer ; ++l){

    float z = (l+0.5)*_cellSizeZ;

	  if( output[ iHit ] > 0. ){

	    ddml::SpacePoint sp(
	      x,
	      y,
	      z,
	      output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  }
	
	  ++iHit ;
	}
      }
    }
  }


/*
void Par04CylindrialScoringMeshVAE::convertOutput(G4FastTrack const& aFastTrack,
            G4ThreeVector const& localDir,
            const std::vector<float>& output,
            std::vector<SpacePointVec>& spacepoints ){
  
    int nLayer = _nCellsZ ; // number of layers is z dimension
    
    spacepoints.resize( nLayer ) ; 

    for(int l=0 ; l < nLayer ; ++l){
      spacepoints[l].reserve( _nCellsRho * _nCellsPhi ) ;
    }

    int iHit = 0 ;

    for(int i=0; i<_nCellsRho; ++i){
      
      float rho = (i+0.5) * _cellSizeRho ;

      for(int j=0; j<_nCellsPhi; ++j){

	float phiCell = (j+0.5) * 2.*CLHEP::pi / _nCellsPhi ;
	float x = rho * cos( phiCell ) ;
	float y = rho * sin( phiCell ) ;
	
  // need to calculate closest sensitive layer for correct attribution of energy



	for(int l=0 ; l < nLayer ; ++l){

	  if( output[ iHit ] > 0. ){

	    ddml::SpacePoint sp(
	      x,
	      y,
	      0.,
	      output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  }
	
	  ++iHit ;
	}
      }
    }
  }
*/

/*
void Par04CylindrialScoringMeshVAE::convertOutput(G4FastTrack const& aFastTrack,
            G4ThreeVector const& localDir,
            const std::vector<float>& output,
            std::vector<SpacePointVec>& spacepoints ){
  
    int nLayer = _nCellsZ ; // number of layers is z dimension
    
    spacepoints.resize( nLayer ) ; 

    for(int l=0 ; l < nLayer ; ++l){
      spacepoints[l].reserve( _nCellsRho * _nCellsPhi ) ;
    }

    int iHit = 0 ;

    for(int i=0; i<_nCellsRho; ++i){
      
      float rho = (i+0.5) * _cellSizeRho ;

      for(int j=0; j<_nCellsPhi; ++j){

	float phiCell = (j+0.5) * 2.*CLHEP::pi / _nCellsPhi ;
	float x = rho * cos( phiCell ) ;
	float y = rho * sin( phiCell ) ;
	
	for(int l=0 ; l < nLayer ; ++l){

	  if( output[ iHit ] > 0. ){

	    ddml::SpacePoint sp(
	      x,
	      y,
	      0.,
	      output[ iHit ] * _initialEnergy ,  // model output is in fraction of initial E
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  }
	
	  ++iHit ;
	}
      }
    }
  }
  */
  
  
}
