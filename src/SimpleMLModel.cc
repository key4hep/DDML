#include "DDML/FastMLShower.h"

#include "DDML/ONNXInference.h"
#include "DDML/RegularGridGANModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/Geant4FastHitMakerGlobal.h"


#include <G4FastStep.hh>                 // for G4FastStep
#include <G4FastTrack.hh>                // for G4FastTrack
#include <G4Track.hh>                    // for G4Track


typedef ddml::ONNXInference Inference ;

typedef ddml::RegularGridGANModel MLModel ;

typedef ddml::PolyhedraBarrelGeometry Geometry ;

typedef Geant4FastHitMakerGlobal HitMaker ;

struct MyFancyMLModel {

  Inference inference ;
  MLModel   model ;
  Geometry  geometry ;
  HitMaker  hitMaker ;
  
  const bool has_constructGeo = false ;
  const bool has_constructField = false ;
  const bool has_constructSensitives = false ;
  const bool has_check_applicability = false ;
  const bool has_check_trigger = false ;

  
  void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

    model.declareProperties( plugin ) ;
    inference.declareProperties( plugin ) ;
    geometry.declareProperties( plugin ) ;
  }

};

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {


    template <typename MyFancyMLModel>
    void FastMLShower<MyFancyMLModel>::modelShower(G4FastTrack const& aFastTrack, G4FastStep& aFastStep){
      
      // remove particle from further processing by G4
      aFastStep.KillPrimaryTrack();
      aFastStep.SetPrimaryTrackPathLength(0.0);
      G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
      aFastStep.SetTotalEnergyDeposited(energy);
 
      
      std::vector<float> input, output ;
      std::vector<ddml::SpacePointVec> spacepoints ;

      fastsimML.model.prepareInput( aFastTrack, input , output ) ;

      fastsimML.inference.runInference(input, output ) ;

      fastsimML.model.convertOutput( aFastTrack, output , spacepoints) ;

      fastsimML.geometry.localToGlobal( aFastTrack, spacepoints ) ;
      
      // now deposit energies in the detector using calculated global positions

      for( auto& layerSPs : spacepoints )
	for( auto& sp : layerSPs ) {
	  fastsimML.hitMaker.make( G4FastHit( G4ThreeVector(sp.X,sp.Y,sp.Z) , sp.E ), aFastTrack);
	}
    }
    
    typedef FastMLShower<MyFancyMLModel> FancyMLShowerModel ;


  }
}


#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(FancyMLShowerModel)
