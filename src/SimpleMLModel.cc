#include <DDG4/Geant4FastSimShowerModel.h>
#include <DDG4/Geant4FastSimShowerModel.inl.h>



struct MyFancyMLModel {

};

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {


    typedef Geant4FSShowerModel<MyFancyMLModel> FancyMLShowerModel ;


  }
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(FancyMLShowerModel)
