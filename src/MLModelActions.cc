#ifdef DDML_USE_TORCH_INFERENCE
// -- includes this before any ROOT stuff - as it defines a struct ClassDef that is otherwise overwritten by Rtypes.h
#include "DDML/TorchInference.h"
#endif

#include "DDML/FastMLShower.h"

#ifdef DDML_USE_ONNX_INFERENCE
#include "DDML/ONNXInference.h"
#endif

#include "DDML/RegularGridGANModel.h"
#include "DDML/RegularGridBIBAEModel.h"
#include "DDML/RegularGridTwoAngleBIBAEModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/EndcapGeometry.h"
#include "DDML/Geant4FastHitMakerGlobal.h"
#include "DDML/Par04ExampleVAE.h"

namespace ddml {

#ifdef DDML_USE_ONNX_INFERENCE
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

  /// Par04 example
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				   ddml::Par04ExampleVAE,
				   ddml::PolyhedraBarrelGeometry,
				   Geant4FastHitMakerGlobal> >
  Par04ExampleVAEPolyhedraBarrelONNXModel ;

  /// a concrete model for regular grid GANs applied to the endcap calorimeter with ONNX
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				   ddml::Par04ExampleVAE,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  Par04ExampleVAEEndcapONNXModel ;
#endif

#ifdef DDML_USE_TORCH_INFERENCE
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

  /// Model for BIBAE regular grid inference in the barrel calorimeter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
                                   ddml::RegularGridBIBAEModel,
                                   ddml::PolyhedraBarrelGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridBIBAEPolyhedraBarrelTorchModel ;

  /// Model for BIBAE regular grid inference in the endcap calorimeter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
				   ddml::RegularGridBIBAEModel,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridBIBAEEndcapTorchModel ;

  /// Model for two angle BIBAE regular grid inference in the barrel calorimeter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
          ddml::RegularGridTwoAngleBIBAEModel,
          ddml::PolyhedraBarrelGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel ;

  /// Model for two angle BIBAE regular grid inference in the endcap calorimter with Torch
  typedef FastMLShower<FastMLModel<ddml::TorchInference,
				   ddml::RegularGridTwoAngleBIBAEModel,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  RegularGridTwoAngleBIBAEModelEndcapTorchModel ;


#endif
}

#include <DDG4/Factories.h>

#ifdef DDML_USE_ONNX_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04ExampleVAEPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04ExampleVAEEndcapONNXModel)
#endif

#ifdef DDML_USE_TORCH_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml,RegularGridBIBAEPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml,RegularGridBIBAEEndcapTorchModel)
#endif
