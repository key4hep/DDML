#include "DDML/RegularGridBIBAEModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "CLHEP/Random/RandGauss.h"     // for RandGauss
#include "CLHEP/Random/RandFlat.h"     // for RandGauss

//#include <torch/script.h>

#define DEBUGPRINT 1

namespace ddml {

    //void RegularGridBIBAEModel::prepareInput(G4FastTrack const& aFastTrack,
     //     std::vector<float>& inputs, TensorDimVecs& tensDims,
      //    std::vector<float>& output ) {

    void RegularGridBIBAEModel::prepareInput(G4FastTrack const& aFastTrack,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) {

      tensDims = _tensDims ;

    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    // here we could use position and direction to compute additional
    // conditioning variables, such as incident angles ...
    // for now assume simple BIBAE with 90 deg incident


    if( DEBUGPRINT ) 
      std::cout << "  RegularGridBIBAEModel::prepareInput   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy / CLHEP::GeV << std::endl ;

    /*
    // create input tensor objects from data values
    std::vector<int64_t> dims_CondE = {1, 1};
    std::vector<int64_t> dims_CondTheta = {1,1};
    */

    /*
    // for now, fake showers with angle 89.0 degrees (avoid edge of training space)
    std::vector<int64_t> Theta_vec(1, 1);
    for(unsigned i=0; i<1; ++i ){
      Theta_vec[i] = 89.*(M_PI/180.);
    }

    // Input[100] is currently the incident energy in GeV

    std::vector<int64_t> E_vec(1, 1);
    E_vec[0] = energy / CLHEP::GeV;
    */

    
    /*
    torch::Tensor ETensor = torch::tensor( E_vec, m_options ).view( dims_CondE );
    torch::Tensor ThetaTensor = torch::tensor( Theta_vec, m_options ).view( dims_CondTheta );

    torch::Tensor CondTensor = torch::cat({ ETensor/100., ThetaTensor/(90.*(M_PI/180.)) }, 1);
    */

    // the input for the BIB-AE is one energy and an angle (plus cond tensor)
    inputs.resize(_latentSize);

    inputs[0].resize(1);
    inputs[1].resize(1);
    inputs[2].resize(2);

    // For now, assume batch size one, and just assign values
    inputs[0][0] = energy / CLHEP::GeV ;//E_vec[0]/100.;
    inputs[1][0] = 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
    inputs[2][0] = ( inputs[0][0] )/100. ;
    inputs[2][1] = ( inputs[1][0] )/ (90.*(M_PI/180.)) ;
    
    // can't include this here
    /*
    torch::Tensor Cond = torch::cat({ inputs[0][0], inputs[1][0] }, 1);


    inputs[2][0] = Cond;
    */


    if(DEBUGPRINT) std::cout << " Input_energy_tensor : " <<   inputs[0][0]*100. << std::endl ;


    /*

    // the input for this model is the latent space and the energy conditioning

        inputs.resize( _latentSize + 1 );

    for(int i = 0; i < _latentSize; ++i)

    {
      inputs[i] = CLHEP::RandFlat::shoot(-1., 1.);

      //    genVector[i] = CLHEP::RandGauss::shoot(0., 1.);
    }

    inputs[ _latentSize ] = energy / CLHEP::GeV ;;

    // ... later ...

    // input[ _latentSize + 1 ] = angle ;
    */
    

    // ----  resize output vector

    int outputSize = _nCellsX * _nCellsY * _nCellsZ ;

    output.assign(outputSize, 0);
  }

  
void RegularGridBIBAEModel::convertOutput(G4FastTrack const& /*aFastTrack*/,
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
	      //( i - int(_nCellsX/2) + 0.5 ) * _cellSizeX ,
	      //( j - int(_nCellsY/2) + 0.5 ) * _cellSizeY ,
        ( i - int(_centerCellX) + 0.5 ) * _cellSizeX ,
        ( j - int(_centerCellY) + 0.5 ) * _cellSizeY ,

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


  