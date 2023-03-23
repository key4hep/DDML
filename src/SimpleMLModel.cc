#include "DDFastShowerML/FastMLShower.h"
#include "DDFastShowerML/ONNXInference.h"

#include <G4FastStep.hh>                 // for G4FastStep
#include <G4FastTrack.hh>                // for G4FastTrack
#include <G4Track.hh>                    // for G4Track


typedef ONNXInference INFERENCE ;



struct MyFancyMLModel {

  INFERENCE inference ;
  
  const bool has_constructGeo = false ;
  const bool has_constructField = false ;
  const bool has_constructSensitives = false ;
  const bool has_check_applicability = false ;
  const bool has_check_trigger = false ;

  int nCellsX = 0 ; 
  int nCellsY = 0 ; 
  int nCellsZ = 0 ; 

  
  void declareProperties( dd4hep::sim::Geant4Action* plugin ) {

    plugin->declareProperty("NCellsX" , this->nCellsX ) ;
    plugin->declareProperty("NCellsY" , this->nCellsY ) ;
    plugin->declareProperty("NCellsZ" , this->nCellsZ ) ;

    inference.declareProperties( plugin ) ;

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
      G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
      G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();
      
      std::cout << "  FastMLShower::modelShower:  pos0 = " << position << " - dir = " << direction << " - E = " << energy << std::endl ;
      
      // calculate the incident angle
      G4float angle = direction.theta();
      
      // calculate how to deposit energy within the detector
      // get it from inference model
      // // fInference->GetEnergies(fEnergies, energy, angle);
      // // fInference->GetPositions(fEnergies, fPositions, position, direction);
      
      std::vector<float> input, output ;
      int outputSize = fastsimML.nCellsX * fastsimML.nCellsY * fastsimML.nCellsZ ;

      fastsimML.inference.runInference(input, output ) ;


      // deposit energy in the detector using calculated values of energy deposits
      // and positions
      for(size_t iHit = 0; iHit < output.size(); iHit++)
      {
//	fHitMaker->make(G4FastHit(fPositions[iHit], fEnergies[iHit]), aFastTrack);
      }
    }
    
    typedef FastMLShower<MyFancyMLModel> FancyMLShowerModel ;


  }
}


#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(FancyMLShowerModel)
