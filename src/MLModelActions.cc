// -- includes this before any ROOT stuff - as it defines a struct ClassDef that is otherwise overwritten by Rtypes.h
#include "DDML/TorchInference.h"

#include "DDML/FastMLShower.h"

#include "DDML/ONNXInference.h"
#include "DDML/RegularGridGANModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/EndcapGeometry.h"
#include "DDML/Geant4FastHitMakerGlobal.h"


namespace ddml {

  /// a concrete model for regular grid GANs applied to the barrel calorimeter with ONNX
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				   ddml::RegularGridGANModel,
				   ddml::PolyhedraBarrelGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridGANPolyhedraBarrelONNXModel ;

  /// a concrete model for regular grid GANs applied to the endcap calorimeter with ONNX
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				   ddml::RegularGridGANModel,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridGANEndcapONNXModel ;

  /// a concrete model for regular grid GANs applied to the barrel calorimeter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
				   ddml::RegularGridGANModel,
				   ddml::PolyhedraBarrelGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridGANPolyhedraBarrelTorchModel ;

  /// a concrete model for regular grid GANs applied to the endcap calorimeter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
				   ddml::RegularGridGANModel,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridGANEndcapTorchModel ;
}


#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANPolyhedraBarrelONNXModel)

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANEndcapONNXModel)

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANPolyhedraBarrelTorchModel)

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANEndcapTorchModel)




