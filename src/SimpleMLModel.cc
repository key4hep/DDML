#include "DDML/FastMLShower.h"

#include "DDML/ONNXInference.h"
#include "DDML/RegularGridGANModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/Geant4FastHitMakerGlobal.h"


typedef ddml::ONNXInference Inference ;

typedef ddml::RegularGridGANModel MLModel ;

typedef ddml::PolyhedraBarrelGeometry Geometry ;

typedef Geant4FastHitMakerGlobal HitMaker ;

struct MyFancyMLModel {

  Inference inference={} ;
  MLModel   model={} ;
  Geometry  geometry={} ;
  HitMaker*  hitMaker={} ;

  MyFancyMLModel() : hitMaker( new HitMaker ) {} 
  
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

namespace dd4hep  {
  namespace sim  {
    typedef FastMLShower<MyFancyMLModel> FancyMLShowerModel ;
  }
}
#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(FancyMLShowerModel)
