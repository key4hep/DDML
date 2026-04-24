#ifdef DDML_USE_TORCH_INFERENCE
// -- includes this before any ROOT stuff - as it defines a struct ClassDef that
// is otherwise overwritten by Rtypes.h
#include "DDML/TorchInference.h"
#endif

#ifdef DDML_USE_EMBEDDED_PYINFERENCE
#include "DDML/EmbeddedPyInference.h"
#endif

#include "DDML/FastMLShower.h"

#ifdef DDML_USE_ONNX_INFERENCE
#include "DDML/ONNXInference.h"
#endif

#ifdef DDML_USE_LOAD_HDF5
#include "DDML/LoadHdf5.h"
#endif

#include "DDML/CaloCloudsTwoAngleModel.h"
#include "DDML/EndcapGeometry.h"
#include "DDML/EndcapTriggerTwoAngleBIBAE.h"
#include "DDML/L2LFlowsModel.h"
#include "DDML/OctogonalBarrelTrigger.h"
#include "DDML/Par04ExampleVAE.h"
#include "DDML/PionCloudsModel.h"
#include "DDML/PolyhedraBarrelGeometry.h"
#include "DDML/RegularGridBIBAEModel.h"
#include "DDML/RegularGridGANModel.h"
#include "DDML/RegularGridTwoAngleBIBAEModel.h"

namespace ddml {

#ifdef DDML_USE_ONNX_INFERENCE
/// a concrete model for regular grid GANs applied to the barrel calorimeter
/// with ONNX
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::RegularGridGANModel, ddml::PolyhedraBarrelGeometry>>
    RegularGridGANPolyhedraBarrelONNXModel;

/// a concrete model for regular grid GANs applied to the endcap calorimeter
/// with ONNX
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::RegularGridGANModel, ddml::EndcapGeometry>>
    RegularGridGANEndcapONNXModel;

/// Par04 example
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::Par04ExampleVAE, ddml::PolyhedraBarrelGeometry>>
    Par04ExampleVAEPolyhedraBarrelONNXModel;

/// a concrete model for regular grid GANs applied to the endcap calorimeter
/// with ONNX
typedef FastMLShower<FastMLModel<ddml::ONNXInference, ddml::Par04ExampleVAE, ddml::EndcapGeometry>>
    Par04ExampleVAEEndcapONNXModel;
#endif

#ifdef DDML_USE_TORCH_INFERENCE
/// a concrete model for regular grid GANs applied to the barrel calorimeter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridGANModel, ddml::PolyhedraBarrelGeometry>>
    RegularGridGANPolyhedraBarrelTorchModel;

/// a concrete model for regular grid GANs applied to the endcap calorimeter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridGANModel, ddml::EndcapGeometry>>
    RegularGridGANEndcapTorchModel;

/// Model for BIBAE regular grid inference in the barrel calorimeter with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridBIBAEModel, ddml::PolyhedraBarrelGeometry>>
    RegularGridBIBAEPolyhedraBarrelTorchModel;

/// Model for BIBAE regular grid inference in the endcap calorimeter with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridBIBAEModel, ddml::EndcapGeometry>>
    RegularGridBIBAEEndcapTorchModel;

/// Model for two angle BIBAE regular grid inference in the barrel calorimeter
/// with Torch
typedef FastMLShower<
    FastMLModel<ddml::TorchInference, ddml::RegularGridTwoAngleBIBAEModel, ddml::PolyhedraBarrelGeometry,
                ddml::OctogonalBarrelTrigger>> // add ML trigger
    RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel;

/// Model for two angle BIBAE regular grid inference in the endcap calorimter
/// with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridTwoAngleBIBAEModel, ddml::EndcapGeometry,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    RegularGridTwoAngleBIBAEModelEndcapTorchModel;

/// CaloClouds Model for the barrel calorimeter with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::CaloCloudsTwoAngleModel, ddml::PolyhedraBarrelGeometry,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    CaloCloudsTwoAngleModelPolyhedraBarrelTorchModel;

/// CaloClouds Model for the endcap calorimeter with Torch
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::CaloCloudsTwoAngleModel, ddml::EndcapGeometry,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    CaloCloudsTwoAngleModelEndcapTorchModel;

/// L2L Flows Model
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::L2LFlowsModel, ddml::PolyhedraBarrelGeometry,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    L2LFlowsModelPolyhedraBarrelTorchModel;
/// L2L Flows Model
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::L2LFlowsModel, ddml::EndcapGeometry,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    L2LFlowsModelEndcapTorchModel;
#endif

#ifdef DDML_USE_EMBEDDED_PYINFERENCE
/// CaloClouds model for the barrel calorimeter with embedded Python (CC3_SF_2A verification example)
typedef FastMLShower<FastMLModel<ddml::EmbeddedPyInference, ddml::CaloCloudsTwoAngleModel,
                                 ddml::PolyhedraBarrelGeometry, ddml::OctogonalBarrelTrigger>>
    CaloCloudsTwoAngleModelPolyhedraBarrelPyEmbeddedModel;
#endif

#ifdef DDML_USE_LOAD_HDF5
/// Load from HDF5 file- as an example for the two angle BIBAE regular grid
// Barrel
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::RegularGridTwoAngleBIBAEModel, ddml::PolyhedraBarrelGeometry,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel;
// Endcap
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::RegularGridTwoAngleBIBAEModel, ddml::EndcapGeometry,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    LoadHDF5RegularGridTwoAngleBIBAEModelEndcap;

/// Load from HDF5 file- as an example for Hadron showers from PionClouds
// Barrel
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::PionCloudsModel, ddml::PolyhedraBarrelGeometry,
                                 ddml::OctogonalBarrelTrigger>> // add ML trigger
    LoadHDF5PionCloudsPCHadronModelPolyhedraBarrel;
// Endcap // ENDCAP IS CURRENTLY NOT IMPLEMENTED!!!!
typedef FastMLShower<FastMLModel<ddml::LoadHdf5, ddml::PionCloudsModel, ddml::EndcapGeometry,
                                 ddml::EndcapTriggerTwoAngleBIBAE>> // add ML trigger
    LoadHDF5PionCloudsPCHadronModelEndcap;
#endif

} // namespace ddml

#include <DDG4/Factories.h>

#ifdef DDML_USE_ONNX_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04ExampleVAEPolyhedraBarrelONNXModel)
DECLARE_GEANT4ACTION_NS(ddml, Par04ExampleVAEEndcapONNXModel)
#endif

#ifdef DDML_USE_TORCH_INFERENCE
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridBIBAEEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, RegularGridTwoAngleBIBAEModelEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, CaloCloudsTwoAngleModelPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, CaloCloudsTwoAngleModelEndcapTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, L2LFlowsModelPolyhedraBarrelTorchModel)
DECLARE_GEANT4ACTION_NS(ddml, L2LFlowsModelEndcapTorchModel)
#endif

#ifdef DDML_USE_EMBEDDED_PYINFERENCE
DECLARE_GEANT4ACTION_NS(ddml, CaloCloudsTwoAngleModelPolyhedraBarrelPyEmbeddedModel)
#endif

#ifdef DDML_USE_LOAD_HDF5
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5RegularGridTwoAngleBIBAEModelEndcap)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5PionCloudsPCHadronModelPolyhedraBarrel)
DECLARE_GEANT4ACTION_NS(ddml, LoadHDF5PionCloudsPCHadronModelEndcap)
#endif
