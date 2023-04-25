#include "DDML/RegularGridGANModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "CLHEP/Random/RandGauss.h"     // for RandGauss
#include "CLHEP/Random/RandFlat.h"     // for RandGauss

#define DEBUGPRINT 1

namespace ddml {



  void RegularGridGANModel::prepareInput(G4FastTrack const& aFastTrack,
					 std::vector<float>& input,
					 std::vector<float>& output ) {



    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    // here we could use position and direction to compute additional
    // conditioning variables, such as incident angles ...
    // for now assume simple GAN with 90 deg incident
    
    if( DEBUGPRINT ) 
      std::cout << "  RegularGridGANModel::prepareInput   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy / CLHEP::GeV << std::endl ;
    
    
    // the input for this model is the latent space and the energy conditioning
    
    input.resize( _latentSize + 1 );

    for(int i = 0; i < _latentSize; ++i)
    {
      input[i] = CLHEP::RandFlat::shoot(-1., 1.);

      //    genVector[i] = CLHEP::RandGauss::shoot(0., 1.);
    }

    input[ _latentSize ] = energy / CLHEP::GeV ;;

    // ... later ...
    // input[ _latentSize + 1 ] = angle ;

    
    // ----  resize output vector
    int outputSize = _nCellsX * _nCellsY * _nCellsZ ;
    
    output.assign(outputSize, 0);
  }
  
  






  void RegularGridGANModel::convertOutput(G4FastTrack const& /*aFastTrack*/,
					  const std::vector<float>& output,
					  std::vector<SpacePointVec>& spacepoints ){

    int nLayer = _nCellsZ ; // number of layers is z dimension
    
    spacepoints.resize( nLayer ) ; 

    int iHit = 0 ;

    for(int l=0 ; l < nLayer ; ++l){

      spacepoints[l].reserve( _nCellsX * _nCellsY ) ;
      
      for(int i=0; i<_nCellsX; ++i){
	for(int j=0; j<_nCellsY; ++j){

	  if( output[ iHit ] > 0. ){

	    ddml::SpacePoint sp(
	      ( i - int(_nCellsX/2) + 0.5 ) * _cellSizeX ,
	      ( j - int(_nCellsY/2) + 0.5 ) * _cellSizeY ,
	      0.,
	      output[ iHit ] ,
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  }
	
	  ++iHit ;
	}
      }
    }
    
  }
}
