#include "DDML/PolyhedraBarrelGeometry.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DDRec/DetectorData.h"


#define DEBUGPRINT 1

namespace ddml {

  PolyhedraBarrelGeometry::PolyhedraBarrelGeometry() {

    auto& theDetector = dd4hep::Detector::getInstance();
    auto det = theDetector.detector( _detector ) ;
    auto* cal = det.extension<dd4hep::rec::LayeredCalorimeterData>() ;
    
    if( cal){
      for( auto l : cal->layers ){

	_caloLayerDistances.push_back(  (l.distance + l.inner_thickness) / dd4hep::mm  ) ;
      }
    
    } else{

      std::cout << " ###### error:  detector " << _detector << " not found !" << std::endl ;
    }
  }


  void PolyhedraBarrelGeometry::localToGlobal(G4FastTrack const& aFastTrack,
					      std::vector<SpacePointVec>& spacepoints ) {



    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
    
    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();
    
    if( DEBUGPRINT ) 
      std::cout << "  PolyhedraBarrelGeometry::localToGlobal   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy << std::endl ;
    
    
    // compute phi sector - 0-7 for octagonal barrel     - TODO : generalise to arbitrary polyhedra
    //         2
    //        --
    //    3 /    \ 1
    //   4  |     | 0
    //    5 \    / 7
    //        --
    //        6
    
    double phi = position.phi() ;
    if( phi < 0. )
      phi = 2. * M_PI + phi ;
    
    int phiSec = phi / (M_PI/8.) ;
    
    if( phiSec == 0 || phiSec == 15 )
      phiSec = 0 ;
    else
      phiSec = (phiSec+1) / 2. ;
    
    
    // --- rotate position and direction to phi sector 0  (calo plane parallel to y-axis at positive x )
    
    G4RotationMatrix rotNeg ;
    rotNeg.rotateZ( - phiSec * M_PI/4. );
    
    G4RotationMatrix rotPos ;
    rotPos.rotateZ( + phiSec * M_PI/4. );
    
    auto posR = rotNeg * position ;
    auto dirR = rotNeg * direction ;

    if( DEBUGPRINT ) 
      std::cout << "  PolyhedraBarrelGeometry::localToGlobal -  position " << position << " - direction " << direction
		<< " phi " << phi <<   " phiSec: " << phiSec
		<< " posR " << posR << " dirR " << dirR 
		<< std::endl ;
    

    // find the first layer that will have signals as sometimes particles are create in the calorimeter !
    int firstLayer = 0 ;
    int nLayer = spacepoints.size() ;
    for(int l= 0; l < nLayer ; ++l ){ 
      double r = _caloLayerDistances[l] ;
      firstLayer = l ;
      // lamda for intersection of particle direction and calo plane in phi sector 0
      double lambda = ( r - posR.x() ) / dirR.x() ;
      if( lambda > 0.)
	break ;
    }

    for(int l= 0; l < nLayer ; ++l ){ 
      
      double r = _caloLayerDistances[ l + firstLayer ] ;
      
      // lamda for intersection of particle direction and calo plane in phi sector 0
      double lambda = ( r - posR.x() ) / dirR.x() ;
      
      G4ThreeVector posC = posR + lambda * dirR ;
      
      for(int i=0, N=spacepoints[l].size(); i<N ; ++i) {
	
	auto& sp = spacepoints[l][i] ;
	
	/// actual local to global:  add intersection point of layer and use global x coordinate as depth in calorimeter
	G4ThreeVector pos( float( _caloLayerDistances[l] ),    // == posC.z() 
			   posC.y() + sp.X ,
			   posC.z() + sp.Y
	  ) ;

	// rotate back to original phi sector
	auto global = rotPos * pos ;

	sp.X = global.x() ;
	sp.Y = global.y() ;
	sp.Z = global.z() ;
	
      }
    }
  }
}
