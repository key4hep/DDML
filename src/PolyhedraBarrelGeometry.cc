#include "DDML/PolyhedraBarrelGeometry.h"

#include <G4FastTrack.hh>                // for G4FastTrack

#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DDRec/DetectorData.h"


#define DEBUGPRINT 1

namespace ddml {

  void PolyhedraBarrelGeometry::initialize(){

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

  int PolyhedraBarrelGeometry::phiSector(G4ThreeVector const& position){

    if( ! _isInitialized ){
      initialize() ;
      _isInitialized = true ;
    }
    
    // compute phi sector, e.g. 0-7 for octagonal barrel
    //         2
    //        --
    //    3 /    \ 1
    //   4 |      | 0
    //    5 \    / 7
    //        --
    //        6
    
    double phi = position.phi() ;
    if( phi < 0. )
      phi = 2. * M_PI + phi ;
    
    int phiSec = phi / (M_PI/_nSymmetry) ;
    
    if( phiSec == 0 || phiSec == (2*_nSymmetry-1) )
      phiSec = 0 ;
    else
      phiSec = (phiSec+1) / 2. ;
    
    return phiSec ;
  }

  G4ThreeVector PolyhedraBarrelGeometry::localDirection(G4FastTrack const& aFastTrack){

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    int phiSec = phiSector( position );

    // --- rotate position and direction to phi sector 0  (calo plane parallel to y-axis at positive x )

    G4RotationMatrix rotNeg ;
    rotNeg.rotateZ( - phiSec * 2. * M_PI/_nSymmetry );

    // new direction in global coordinates
    auto dirR = rotNeg * direction ;

    // now transform this direction into a right handed coordinate system that has the z-axis pointing into the calo
    G4ThreeVector localDir( - dirR.z() , dirR.y() , dirR.x() );

    if( DEBUGPRINT ) {
      G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

      std::cout << "  PolyhedraBarrelGeometry::localDirection  - symmetry = " << _nSymmetry <<  " pos0 = " << position
		<< " - dir = " << direction << " - E = "
		<< " - localDir = " << localDir
		<< energy << std::endl ;
      std::cout << "  PolyhedraBarrelGeometry::localDirection  - phi = " << atan2( localDir.y() , localDir.x() ) / M_PI * 180.
		<< "   theta : " << acos( localDir.z() ) / M_PI * 180.
		<< std::endl ;
    }

    return localDir ;
  }

  void PolyhedraBarrelGeometry::localToGlobal(G4FastTrack const& aFastTrack,
					      std::vector<SpacePointVec>& spacepoints ) {

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    int phiSec = phiSector( position );

    if( DEBUGPRINT ) {
      G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

      std::cout << "  PolyhedraBarrelGeometry::localToGlobal  - symmetry = " << _nSymmetry <<  " pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy << std::endl ;
    }

    // --- rotate position and direction to phi sector 0  (calo plane parallel to y-axis at positive x )

    G4RotationMatrix rotNeg ;
    rotNeg.rotateZ( - phiSec * 2. * M_PI/_nSymmetry );

    G4RotationMatrix rotPos ;
    rotPos.rotateZ( + phiSec * 2. * M_PI/_nSymmetry );

    auto posR = rotNeg * position ;
    auto dirR = rotNeg * direction ;

    const bool correctForAngles = false ; //FIXME: make parameter

    if( ! correctForAngles )
      dirR = { 1., 0. , 0.  } ;  // position layers w/ impact normal to the plane

    if( DEBUGPRINT ) 
      std::cout << "  PolyhedraBarrelGeometry::localToGlobal -  position " << position << " - direction " << direction
		<< " phiSec: " << phiSec
		<< " posR " << posR << " dirR " << dirR 
		<< std::endl ;

    // find the first layer that will have signals as sometimes particles are create in the calorimeter !
    int firstLayer = 0 ;
    int nLayer = _caloLayerDistances.size() ;

    for(int l= 0; l < nLayer ; ++l ){ 
      double r = _caloLayerDistances[l] ;
      firstLayer = l ;
      // lamda for intersection of particle direction and calo plane in phi sector 0
      double lambda = ( r - posR.x() ) / dirR.x() ;
      if( lambda > 0.)
	break ;
    }

    nLayer -= firstLayer ; // only populate existing calo layers

    // and remove other layers
    for(unsigned k=nLayer ; k< spacepoints.size() ; ++k )
      spacepoints[k].clear() ;

    for(int l= 0; l < nLayer ; ++l ){ 
      
      double r = _caloLayerDistances[ l + firstLayer ] ;
      
      // lamda for intersection of particle direction and calo plane in phi sector 0
      double lambda = ( r - posR.x() ) / dirR.x() ;
      
      G4ThreeVector posC = posR + lambda * dirR ;
      
      for(int i=0, N=spacepoints[l].size(); i<N ; ++i) {
	
	auto& sp = spacepoints[l][i] ;
	
	/// actual local to global:  add intersection point of layer and use global x coordinate as depth in calorimeter
	// take coordinate transform into account:   z=-x' (see localDirection() )
 
	G4ThreeVector pos( float( _caloLayerDistances[l] ),    // == posC.z() 
			   posC.y() + sp.Y ,
			   posC.z() - sp.X
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
