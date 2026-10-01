#!/usr/bin/env python3

from .model import ModelConfig
from .geometry import PluginGeometry, DetectorGeometry

from g4units import GeV

_ILD_ECAL_BARREL = DetectorGeometry(
    detector="EcalBarrel", region="EcalBarrelRegion", symmetry=8
)
_ILD_ECAL_ENDCAP = DetectorGeometry(detector="EcalEndcap", region="EcalEndcapRegion")
_ILD_HCAL_BARREL = DetectorGeometry(
    detector="HcalBarrel", region="HcalBarrelRegion", symmetry=8
)
_ILD_HCAL_ENDCAP = DetectorGeometry(detector="HcalEndcap", region="HcalEndcapRegion")

ILD_BARREL = PluginGeometry(ecal=_ILD_ECAL_BARREL, hcal=_ILD_HCAL_BARREL)
ILD_ENDCAP = PluginGeometry(ecal=_ILD_ECAL_ENDCAP, hcal=_ILD_HCAL_ENDCAP)


PHOTONS = frozenset({"gamma"})
PHOTON_TRIGGER_5_GEV = {"gamma": 5.0 * GeV}
PHOTON_TRIGGER_10_GEV = {"gamma": 10.0 * GeV}


_cc3_common_properties = {
    "OptimizeFlag": 1,
    "IntraOpNumThreads": 1,
    "ModelPath": "../models/CC3_paper_checkpoint.pt",
}

_bibae_common_properties = {
    "OptimizeFlag": 1,
    "IntraOpNumThreads": 1,
    "ModelPath": "../models/BIBAE_Two_Angle_Full_PP_cut.pt",
}

# CaloClouds3 default configuration
CC3_BARREL = ModelConfig(
    plugin="CaloCloudsTwoAngleModelPolyhedraBarrelTorchModel/BarrelModelTorch",
    geometry=ILD_BARREL,
    plugin_properties=_cc3_common_properties,
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)
CC3_ENDCAP = ModelConfig(
    plugin="CaloCloudsTwoAngleModelEndcapTorchModel/EndcapTorchModel",
    geometry=ILD_ENDCAP,
    plugin_properties=_cc3_common_properties,
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)

# BIBAE (with two angle capabilities) default configuration
BIBAE_BARREL = ModelConfig(
    plugin="RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel/BarrelModelTorch",
    geometry=ILD_BARREL,
    plugin_properties=_bibae_common_properties,
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)
BIBAE_ENDCAP = ModelConfig(
    plugin="RegularGridTwoAngleBIBAEModelEndcapTorchModel/EndcapModelTorch",
    geometry=ILD_ENDCAP,
    plugin_properties=_bibae_common_properties,
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)


# # ---------------------------------------------------------------------------
# # ONNX
# # ---------------------------------------------------------------------------

# PAR04_VAE = ShowerPreset(
#     barrel_plugin="Par04ExampleVAEPolyhedraBarrelONNXModel/ShowerModel",
#     endcap_plugin="Par04ExampleVAEEndcapONNXModel/ShowerModel",
#     model_file="../models/Generator.onnx",
#     file_attr="ModelPath",
#     applicable_particles=EM_PARTICLES,
#     etrigger_gev=EM_TRIGGER_5_GEV,
#     correct_angles=True,
#     optimize_flag=1,
# )

# # Name clash with Torch backend — suffix required
# REGULAR_GRID_GAN_ONNX = ShowerPreset(
#     barrel_plugin="RegularGridGANPolyhedraBarrelONNXModel/ShowerModel",
#     endcap_plugin="RegularGridGANEndcapONNXModel/ShowerModel",
#     model_file="../models/francisca_gan.onnx",
#     file_attr="ModelPath",
#     applicable_particles=EM_PARTICLES,
#     etrigger_gev=EM_TRIGGER_5_GEV,
#     correct_angles=True,
#     optimize_flag=1,
# )

# # ---------------------------------------------------------------------------
# # Torch
# # ---------------------------------------------------------------------------


# L2L_FLOWS = ShowerPreset(
#     barrel_plugin="L2LFlowsModelPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="L2LFlowsModelEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/L2LFlowsx9.pt",
#     file_attr="ModelPath",
#     applicable_particles=EM_PARTICLES,
#     etrigger_gev=EM_TRIGGER_10_GEV,
#     correct_angles=True,
#     optimize_flag=1,
#     intra_op_threads=1,
# )

# # Name clash with ONNX backend — suffix required
# REGULAR_GRID_GAN_TORCH = ShowerPreset(
#     barrel_plugin="RegularGridGANPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="RegularGridGANEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/francisca_gan_jit.pt",
#     file_attr="ModelPath",
#     applicable_particles=EM_PARTICLES,
#     etrigger_gev=EM_TRIGGER_10_GEV,
#     correct_angles=True,
#     optimize_flag=1,
#     intra_op_threads=1,
# )

# # ---------------------------------------------------------------------------
# # HDF5  (_HDF5 suffix always included)
# # ---------------------------------------------------------------------------

# BIBAE_TWO_ANGLE_HDF5 = ShowerPreset(
#     barrel_plugin="LoadHDF5RegularGridTwoAngleBIBAEModelPolyhedraBarrel/BarrelModelTorch",
#     endcap_plugin="LoadHDF5RegularGridTwoAngleBIBAEModelEndcap/EndcapModelTorch",
#     model_file="../models/photons-E5050A-theta9090A-phi9090-p1.hdf5",
#     file_attr="FilePath",
#     applicable_particles=EM_PARTICLES,
#     etrigger_gev=EM_TRIGGER_10_GEV,
#     correct_angles=False,
# )

# BIBAE_TWO_ANGLE_ENDCAP_ONLY_HDF5 = replace(BIBAE_TWO_ANGLE_HDF5, barrel_plugin=None)

# PION_CLOUDS_HADRON_HDF5 = ShowerPreset(
#     barrel_plugin="LoadHDF5PionCloudsPCHadronModelPolyhedraBarrel/BarrelModelTorch",
#     endcap_plugin=None,
#     model_file="../models/PionClouds_50GeV_sp_scaled.h5",
#     file_attr="FilePath",
#     applicable_particles=frozenset({"pi+"}),
#     etrigger_gev={"pi+": 10.0},
#     correct_angles=False,
#     is_hadron=True,
# )
