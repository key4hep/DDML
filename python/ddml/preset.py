from dataclasses import dataclass
from typing import Mapping, Any

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
