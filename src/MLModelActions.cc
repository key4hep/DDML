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
#include "DDML/EndcapGeometryParallelWorld.h"
#include "DDML/EndcapTriggerTwoAngleBIBAE.h"
#include "DDML/Geant4FastHitMakerGlobal.h"
#include "DDML/OctogonalBarrelTrigger.h"
#include "DDML/Par04CylindrialScoringMeshVAE.h"
#include "DDML/Par04ExampleVAE.h"
#include "DDML/ParallelWorldCaloDiTModel.h"
#include "DDML/ParallelWorldALLEGROCaloDiTModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/PolyhedraBarrelGeometryParallelWorld.h"
#include "DDML/CylindricalBarrelGeometryParallelWorld.h"
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
typedef FastMLShower<
    FastMLModel<ddml::ONNXInference, ddml::Par04ExampleVAE, ddml::EndcapGeometry, Geant4FastHitMakerGlobal>>
    Par04ExampleVAEEndcapONNXModel;

/// Cylindrical Mesh Parallel World Geometry
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::Par04CylindrialScoringMeshVAE,
                                 ddml::PolyhedraBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    Par04CylindrialScoringMeshVAEBarrelParallelONNXModel;

// Endcap copy Par04Example- update once barrel checked!
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::Par04CylindrialScoringMeshVAE,
                                 ddml::EndcapGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    Par04CylindrialScoringMeshVAEEndcapParallelONNXModel;

/// FCCee CLD barrel onnx inference
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::ParallelWorldCaloDiTModel,
                                 ddml::PolyhedraBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    ParallelWorldCaloDiTModelBarrelParallelONNXModel;

// FCCee CLD endcap ? onnx inference
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::ParallelWorldCaloDiTModel,
                                 ddml::EndcapGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    ParallelWorldCaloDiTModelEndcapParallelONNXModel;

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
/// FCCee CLD barrel torch inference
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::ParallelWorldCaloDiTModel,
                                 ddml::PolyhedraBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    ParallelWorldCaloDiTModelBarrelParallelTorchModel;

// FCCee CLD endcap ? torch inference
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::ParallelWorldCaloDiTModel,
                                 ddml::EndcapGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    ParallelWorldCaloDiTModelEndcapParallelTorchModel;

//FCCee ALLEGRO barrel torch inference
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::ParallelWorldCaloDiTModel,
                                ddml::CylindricalBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    ParallelWorldCaloDiTModelCylindricalBarrelTorchModel;
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
/// FCCee CLD barrel hdf5 loading
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::ParallelWorldCaloDiTModel,
                                 ddml::PolyhedraBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    LoadHDF5ParallelWorldCaloDiTModelBarrelParallel;

// FCCee CLD endcap ? hdf5 loading
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::ParallelWorldCaloDiTModel,
                                 ddml::EndcapGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    LoadHDF5ParallelWorldCaloDiTModelEndcapParallel;

// FCCee ALLEGRO barrel hdf5 loading
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::ParallelWorldALLEGROCaloDiTModel,
                                ddml::CylindricalBarrelGeometryParallelWorld, Geant4FastHitMakerGlobal>>
    LoadHDF5ParallelWorldCaloDiTModelCylindricalBarrel;

#endif

} // namespace ddml

#include <DDG4/Factories.h>

#ifdef DDML_USE_ONNX_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04ExampleVAEPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04ExampleVAEEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04CylindrialScoringMeshVAEBarrelParallelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04CylindrialScoringMeshVAEEndcapParallelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, ParallelWorldCaloDiTModelBarrelParallelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, ParallelWorldCaloDiTModelEndcapParallelONNXModel)
#endif

#ifdef DDML_USE_TORCH_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, ParallelWorldCaloDiTModelBarrelParallelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, ParallelWorldCaloDiTModelEndcapParallelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, ParallelWorldCaloDiTModelCylindricalBarrelTorchModel)
#endif

#ifdef DDML_USE_LOAD_HDF5
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelEndcap)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5ParallelWorldCaloDiTModelBarrelParallel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5ParallelWorldCaloDiTModelEndcapParallel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5ParallelWorldCaloDiTModelCylindricalBarrel)
#endif
