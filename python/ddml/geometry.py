#!/usr/bin/env python3
from dataclasses import dataclass

from typing import Optional


@dataclass(frozen=True)
class DetectorGeometry:
    detector: str  # (sub)detector name for placing hits
    region: str  # The (detector) region for which the model should be applied
    symmetry: Optional[int] = None  # The symmetry (if applicable) of the geometry


_ILD_ECAL_BARREL = DetectorGeometry(
    detector="EcalBarrel", region="EcalBarrelRegion", symmetry=8
)
_ILD_ECAL_ENDCAP = DetectorGeometry(detector="EcalEndcap", region="EcalEndcapRegion")
_ILD_HCAL_BARREL = DetectorGeometry(
    detector="HcalBarrel", region="HcalBarrelRegion", symmetry=8
)
_ILD_HCAL_ENDCAP = DetectorGeometry(detector="HcalEndcap", region="HcalEndcapRegion")


@dataclass(frozen=True)
class PluginGeometry:
    ecal: DetectorGeometry
    hcal: Optional[DetectorGeometry] = None


ILD_BARREL = PluginGeometry(ecal=_ILD_ECAL_BARREL, hcal=_ILD_HCAL_BARREL)
ILD_ENDCAP = PluginGeometry(ecal=_ILD_ECAL_ENDCAP, hcal=_ILD_HCAL_ENDCAP)
