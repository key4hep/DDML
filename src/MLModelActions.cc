#ifdef DDML_USE_TORCH_INFERENCE
  // -- includes this before any ROOT stuff - as it defines a struct ClassDef that
  // is otherwise overwritten by Rtypes.h
  #include "DDML/TorchInference.h"
#endif

#include "DDML/FastMLShower.h"

#ifdef DDML_USE_ONNX_INFERENCE
  #include "DDML/ONNXInference.h"
#endif

#ifdef DDML_USE_LOAD_HDF5
  #include "DDML/LoadHdf5.h"
#endif

#include "DDML/EndcapGeometry.h"
#include "DDML/PolyhedraBarrelGeometryParallelWorld.h"
#include "DDML/Geant4FastHitMakerGlobal.h"
#include "DDML/Par04ExampleVAE.h"
#include "DDML/Par04CylindrialScoringMeshVAE.h"
#include "DDML/EndcapTriggerTwoAngleBIBAE.h"
#include "DDML/Geant4FastHitMakerGlobal.h"
#include "DDML/OctogonalBarrelTrigger.h"
#include "DDML/Par04ExampleVAE.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/RegularGridBIBAEModel.h"
#include "DDML/RegularGridGANModel.h"
#include "DDML/RegularGridTwoAngleBIBAEModel.h"
#include "DDML/TriggerInterface.h"

namespace ddml {

#ifdef DDML_USE_ONNX_INFERENCE
/// a concrete model for regular grid GANs applied to the barrel calorimeter
/// with ONNX
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::RegularGridGANModel, ddml::PolyhedraBarrelGeometry,
                                 Geant4FastHitMakerGlobal>>
    RegularGridGANPolyhedraBarrelONNXModel;

/// a concrete model for regular grid GANs applied to the endcap calorimeter
/// with ONNX
typedef FastMLShower<
    FastMLModel<ddml::ONNXInference, ddml::RegularGridGANModel, ddml::EndcapGeometry, Geant4FastHitMakerGlobal>>
    RegularGridGANEndcapONNXModel;

/// Par04 example
typedef FastMLShower<
    FastMLModel<ddml::ONNXInference, ddml::Par04ExampleVAE, ddml::PolyhedraBarrelGeometry, Geant4FastHitMakerGlobal>>
    Par04ExampleVAEPolyhedraBarrelONNXModel;

  /// a concrete model for regular grid GANs applied to the endcap calorimeter with ONNX
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				   ddml::Par04ExampleVAE,
				   ddml::EndcapGeometry,
				   Geant4FastHitMakerGlobal> >
  Par04ExampleVAEEndcapONNXModel ;


  /// Cylindrical Mesh Parallel World Geometry
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
          ddml::Par04CylindrialScoringMeshVAE,
          ddml::PolyhedraBarrelGeometryParallelWorld,
          Geant4FastHitMakerGlobal> >
  Par04CylindrialScoringMeshVAEBarrelParallelONNXModel;

// Endcap copy Par04Example- update once barrel checked!
  typedef FastMLShower<FastMLModel<ddml::ONNXInference,
				  ddml::Par04CylindrialScoringMeshVAE,
				  ddml::EndcapGeometry,
				  Geant4FastHitMakerGlobal> >
  Par04CylindrialScoringMeshVAEEndcapParallelONNXModel ;

#endif

#ifdef DDML_USE_TORCH_INFERENCE
/// a concrete model for regular grid GANs applied to the barrel calorimeter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridGANModel, ddml::PolyhedraBarrelGeometry,
                                 Geant4FastHitMakerGlobal>>
    RegularGridGANPolyhedraBarrelTorchModel;

/// a concrete model for regular grid GANs applied to the endcap calorimeter
/// with Torch
typedef FastMLShower<
    FastMLModel<ddml::TorchInference, ddml::RegularGridGANModel, ddml::EndcapGeometry, Geant4FastHitMakerGlobal>>
    RegularGridGANEndcapTorchModel;

/// Model for BIBAE regular grid inference in the barrel calorimeter with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridBIBAEModel, ddml::PolyhedraBarrelGeometry,
                                 Geant4FastHitMakerGlobal>>
    RegularGridBIBAEPolyhedraBarrelTorchModel;

/// Model for BIBAE regular grid inference in the endcap calorimeter with Torch
typedef FastMLShower<
    FastMLModel<ddml::TorchInference, ddml::RegularGridBIBAEModel, ddml::EndcapGeometry, Geant4FastHitMakerGlobal>>
    RegularGridBIBAEEndcapTorchModel;

/// Model for two angle BIBAE regular grid inference in the barrel calorimeter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridTwoAngleBIBAEModel,
                                 ddml::PolyhedraBarrelGeometry, Geant4FastHitMakerGlobal,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel;

/// Model for two angle BIBAE regular grid inference in the endcap calorimter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridTwoAngleBIBAEModel, ddml::EndcapGeometry,
                                 Geant4FastHitMakerGlobal,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    RegularGridTwoAngleBIBAEModelEndcapTorchModel;
#endif

#ifdef DDML_USE_LOAD_HDF5
/// Load from HDF5 file- as an example for the two angle BIBAE regular grid
// Barrel
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::RegularGridTwoAngleBIBAEModel, ddml::PolyhedraBarrelGeometry,
                                 Geant4FastHitMakerGlobal,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel;
// Endcap
typedef FastMLShower<
    FastMLModel<ddml::LoadHdf5, ddml::RegularGridTwoAngleBIBAEModel, ddml::EndcapGeometry, Geant4FastHitMakerGlobal,
                ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    LoadHDF5RegularGridTwoAngleBIBAEModelEndcap;
#endif

} // namespace ddml

#include <DDG4/Factories.h>

#ifdef DDML_USE_ONNX_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,RegularGridGANEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04ExampleVAEPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04ExampleVAEEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04CylindrialScoringMeshVAEBarrelParallelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml,Par04CylindrialScoringMeshVAEEndcapParallelONNXModel)
#endif

#ifdef DDML_USE_TORCH_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelEndcapTorchModel)
#endif

#ifdef DDML_USE_LOAD_HDF5
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelEndcap)
#endif
