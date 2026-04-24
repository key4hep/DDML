#!/usr/bin/env python3
from dataclasses import dataclass

from typing import Optional


@dataclass(frozen=True)
class DetectorGeometry:
    detector: str  # (sub)detector name for placing hits
    region: str  # The (detector) region for which the model should be applied
    symmetry: Optional[int] = None  # The symmetry (if applicable) of the geometry


@dataclass(frozen=True)
class PluginGeometry:
    ecal: DetectorGeometry
    hcal: Optional[DetectorGeometry] = None
