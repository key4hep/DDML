import argparse
import sys
from dataclasses import dataclass
from typing import Mapping, Any, List

from .geometry import PluginGeometry


@dataclass(frozen=True)
class ModelPreset:
    plugin: str  # The DDML plugin name
    plugin_properties: Mapping[str, Any]  # The plugin properties
    geometry: PluginGeometry  # The geometry info for this preset
    applicable_particles: frozenset  # e.g. frozenset({"e+","e-","gamma"})
    # Trigger configuratoin (particle: energy). NOTE: Units are user responsibility!
    triggers: Mapping[str, float]
    correct_angles: bool = False  # Whether or not to use angle correction
    is_hadron: bool = False  # Whether or not this is a hadronic model or not


def add_shower_model(kernel, preset: ModelPreset) -> None:
    # DO NOT MOVE — GeV must resolve inside the callback so Etrigger gets the right unit factor.
    from DDG4 import DetectorConstruction, Geant4

    geometry = preset.geometry

    seq = Geant4(kernel).detectorConstruction()
    m = DetectorConstruction(kernel, preset.plugin)
    # For now many things still hinge on the ecal, we use that as default
    m.RegionName = geometry.ecal.region
    m.Detector = geometry.ecal.detector
    if geometry.ecal.symmetry is not None:
        m.Symmetry = geometry.ecal.symmetry

    # Only if we deal with a hadron shower do we actually use the hcal geometry
    # information
    if preset.is_hadron:
        m.isHadShower = True
        m.HadDetector = geometry.hcal.detector
        if geometry.hcal.symmetry is not None:
            m.HadSymmetry = geometry.hcal.symmetry

    # Set all the other (less standardized) properties
    for prop, val in preset.plugin_properties.items():
        setattr(m, prop, val)

    m.Enable = True
    m.CorrectForAngles = preset.correct_angles
    m.ApplicableParticles = set(preset.applicable_particles)
    m.Etrigger = preset.triggers
    m.enableUI()
    seq.adopt(m)


def user_physics(presets: List[ModelPreset], verbose: bool = True):
    """Return a callable compatible with SIM.physics.setupUserPhysics()."""
    if not presets:
        raise ValueError("user_physics requires at least one ShowerPreset")

    def _setup(kernel):
        from DDG4 import PhysicsList

        for p in presets:
            add_shower_model(kernel, p)
        particles = sorted({x for p in presets for x in p.applicable_particles})
        phys = kernel.physicsList()
        ph = PhysicsList(kernel, "Geant4FastPhysics/FastPhysicsList")
        ph.EnabledParticles = particles
        ph.BeVerbose = verbose
        ph.enableUI()
        phys.adopt(ph)
        phys.dump()

    return _setup


_CLI = argparse.ArgumentParser(add_help=False)
_CLI.add_argument(
    "--ml-preset",
    action="append",
    default=None,
    help="Preset dotted name (e.g. torch.CALOCLOUDS). Repeat to compose.",
)
