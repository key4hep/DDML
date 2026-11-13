#!/usr/bin/env python3

from .model import ModelConfig
from .geometry import PluginGeometry, DetectorGeometry
from .paths import model_path

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

ALLSHOWERS_PARTICLES = frozenset(
    {
        "gamma",
        "e-",
        "e+",
        "pi-",
        "pi+",
        "kaon-",
        "kaon+",
        "kaon0L",
        "proton",
        "anti_proton",
        "neutron",
        "anti_neutron",
    }
)
ALLSHOWERS_TRIGGERS = {name: 10.0 * GeV for name in ALLSHOWERS_PARTICLES}


_cc3_common_properties = {
    "OptimizeFlag": 1,
    "IntraOpNumThreads": 1,
    "ModelPath": model_path("CC3_paper_checkpoint.pt"),
}

_bibae_common_properties = {
    "OptimizeFlag": 1,
    "IntraOpNumThreads": 1,
    "ModelPath": model_path("BIBAE_Two_Angle_Full_PP_cut.pt"),
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

AS_BARREL_PY_INTERFACE = ModelConfig(
    plugin="MultiParticleModelPolyhedraBarrelPyEmbeddedModel/BarrelModelPython",
    geometry=ILD_BARREL,
    plugin_properties={
        "PythonModule": "allshowers_wrapper",
        "EntryPoint": "run_inference",
        "NumberOfLayers": 78,
    },
    correct_angles=False,
    applicable_particles=ALLSHOWERS_PARTICLES,
    triggers=ALLSHOWERS_TRIGGERS,
    is_hadron=True,  # fine even if not always hadron. Just makes HCAL exist
)

AS_ENDCAP_PY_INTERFACE = ModelConfig(
    plugin="MultiParticleModelEndcapPyEmbeddedModel/EndcapModelPython",
    geometry=ILD_ENDCAP,
    plugin_properties={
        "PythonModule": "allshowers_wrapper",
        "EntryPoint": "run_inference",
        "NumberOfLayers": 78,
    },
    correct_angles=False,
    applicable_particles=ALLSHOWERS_PARTICLES,
    triggers=ALLSHOWERS_TRIGGERS,
    is_hadron=True,  # fine even if not always hadron. Just makes HCAL exist
)

CC3_BARREL_PY_INTERFACE = ModelConfig(
    plugin="MultiParticleModelPolyhedraBarrelPyEmbeddedModel/BarrelModelPython",
    geometry=ILD_BARREL,
    plugin_properties={
        "PythonModule": "cc3_sf_2a_wrapper",
        "EntryPoint": "run_inference",
        "NumberOfLayers": 30,
    },
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)

CC3_ENDCAP_PY_INTERFACE = ModelConfig(
    plugin="MultiParticleModelEndcapPyEmbeddedModel/EndcapModelPython",
    geometry=ILD_ENDCAP,
    plugin_properties={
        "PythonModule": "cc3_sf_2a_wrapper",
        "EntryPoint": "run_inference",
        "NumberOfLayers": 30,
    },
    correct_angles=False,
    applicable_particles=PHOTONS,
    triggers=PHOTON_TRIGGER_10_GEV,
)
