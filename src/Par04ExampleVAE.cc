#include "DDML/Par04ExampleVAE.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "CLHEP/Random/RandGauss.h"     // for RandGauss
#include "CLHEP/Random/RandFlat.h"     // for RandGauss

#define DEBUGPRINT 1

namespace ddml {



  void Par04ExampleVAE::prepareInput(G4FastTrack const& aFastTrack,
					 InputVecs& inputs, TensorDimVecs& tensDims,
					 std::vector<float>& output ) {

    tensDims = _tensDims ;

    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    // here we could use position and direction to compute additional
    // conditioning variables, such as incident angles ...
    // for now assume simple GAN with 90 deg incident
    
    if( DEBUGPRINT ) 
      std::cout << "  Par04ExampleVAE::prepareInput   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy / CLHEP::GeV << std::endl ;
    
    
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
    float fMaxAngle = 90.0;
    
    inputs[0][_latentSize ]     = (energy /CLHEP::MeV) / fMaxEnergy ;
    inputs[0][_latentSize + 1 ] = 1. ; //( direction.theta() /CLHEP::deg) / fMaxAngle ;
    inputs[0][_latentSize + 2 ] = 0 ;
    inputs[0][_latentSize + 3 ] = 1 ;

    _initialEnergy = energy /CLHEP::MeV ;
    
    // ----  resize output vector
    int outputSize = _nCellsRho * _nCellsPhi * _nCellsZ ;
    
    output.assign(outputSize, 0);
  }
  
  






  void Par04ExampleVAE::convertOutput(G4FastTrack const& /*aFastTrack*/,
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

	float phi = (j+0.5) * 2.*CLHEP::pi / _nCellsPhi ;
	float x = rho * cos( phi ) ;
	float y = rho * sin( phi ) ;
	
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
}
