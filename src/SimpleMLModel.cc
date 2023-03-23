#include "DDFastShowerML/FastMLShower.h"
#include "DDFastShowerML/ONNXInference.h"


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
    void FastMLShower<MyFancyMLModel>::modelShower(G4FastTrack const&, G4FastStep&){


    }


    
    typedef FastMLShower<MyFancyMLModel> FancyMLShowerModel ;
  }
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(FancyMLShowerModel)
