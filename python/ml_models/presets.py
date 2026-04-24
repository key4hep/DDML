from .common import ModelPreset
from .geometry import ILD_BARREL, ILD_ENDCAP

from g4units import GeV

_EM = frozenset({"e+", "e-", "gamma"})
_TRIG_5 = {"e+": 5.0 * GeV, "e-": 5.0 * GeV, "gamma": 5.0 * GeV}
_TRIG_10 = {"e+": 10.0 * GeV, "e-": 10.0 * GeV, "gamma": 10.0 * GeV}


_cc3_common_properties = {
    "OptimizeFlag": 1,
    "IntraOpNumThreads": 1,
    "ModelPath": "../models/CC3_SF_2A.pt",
}

CC3_BARREL = ModelPreset(
    plugin="CaloCloudsTwoAngleModelPolyhedraBarrelTorchModel/BarrelModelTorch",
    geometry=ILD_BARREL,
    plugin_properties=_cc3_common_properties,
    correct_angles=False,
    applicable_particles=_EM,
    triggers=_TRIG_10,
)

CC3_ENDCAP = ModelPreset(
    plugin="CaloCloudsTwoAngleModelEndcapTorchModel/EndcapTorchModel",
    geometry=ILD_ENDCAP,
    plugin_properties=_cc3_common_properties,
    correct_angles=False,
    applicable_particles=_EM,
    triggers=_TRIG_10,
)


# # ---------------------------------------------------------------------------
# # ONNX
# # ---------------------------------------------------------------------------

# PAR04_VAE = ShowerPreset(
#     barrel_plugin="Par04ExampleVAEPolyhedraBarrelONNXModel/ShowerModel",
#     endcap_plugin="Par04ExampleVAEEndcapONNXModel/ShowerModel",
#     model_file="../models/Generator.onnx",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_5,
#     correct_angles=True,
#     optimize_flag=1,
# )

# # Name clash with Torch backend — suffix required
# REGULAR_GRID_GAN_ONNX = ShowerPreset(
#     barrel_plugin="RegularGridGANPolyhedraBarrelONNXModel/ShowerModel",
#     endcap_plugin="RegularGridGANEndcapONNXModel/ShowerModel",
#     model_file="../models/francisca_gan.onnx",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_5,
#     correct_angles=True,
#     optimize_flag=1,
# )

# # ---------------------------------------------------------------------------
# # Torch
# # ---------------------------------------------------------------------------

# BIBAE = ShowerPreset(
#     barrel_plugin="RegularGridBIBAEPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="RegularGridBIBAEEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/BIBAE_Full_PP_cut.pt",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
#     correct_angles=False,
#     optimize_flag=1,
#     intra_op_threads=1,
# )

# BIBAE_TWO_ANGLE = ShowerPreset(
#     barrel_plugin="RegularGridTwoAngleBIBAEModelPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="RegularGridTwoAngleBIBAEModelEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/BIBAE_Two_Angle_Full_PP_cut.pt",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
#     correct_angles=False,
#     optimize_flag=1,
#     intra_op_threads=1,
# )


# CALOCLOUDS = ShowerPreset(
#     barrel_plugin="CaloCloudsTwoAngleModelPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="CaloCloudsTwoAngleModelEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/CC3_SF_2A.pt",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
#     correct_angles=False,
#     optimize_flag=1,
#     intra_op_threads=1,
# )

# L2L_FLOWS = ShowerPreset(
#     barrel_plugin="L2LFlowsModelPolyhedraBarrelTorchModel/BarrelModelTorch",
#     endcap_plugin="L2LFlowsModelEndcapTorchModel/EndcapModelTorch",
#     model_file="../models/L2LFlowsx9.pt",
#     file_attr="ModelPath",
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
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
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
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
#     applicable_particles=_EM,
#     etrigger_gev=_TRIG_10,
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
