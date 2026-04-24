import argparse
import json
import os
import sys
from dataclasses import dataclass, replace
from typing import Mapping, Optional, Tuple


@dataclass(frozen=True)
class Geometry:
    ecal_barrel: str
    ecal_endcap: str
    ecal_barrel_symmetry: int
    hcal_barrel: str
    hcal_endcap: str
    hcal_barrel_symmetry: int


ILD = Geometry(
    ecal_barrel="EcalBarrel",
    ecal_endcap="EcalEndcap",
    ecal_barrel_symmetry=8,
    hcal_barrel="HcalBarrel",
    hcal_endcap="HcalEndcap",
    hcal_barrel_symmetry=8,
)
NON_ILD = Geometry(
    ecal_barrel="ECalBarrel",
    ecal_endcap="ECalEndcap",
    ecal_barrel_symmetry=12,
    hcal_barrel="HCalBarrel",
    hcal_endcap="HCalEndcap",
    hcal_barrel_symmetry=12,
)
_GEOMETRIES = {"ILD": ILD, "NON_ILD": NON_ILD}


@dataclass(frozen=True)
class ShowerPreset:
    barrel_plugin: Optional[str]  # None -> skip barrel instance
    endcap_plugin: Optional[str]  # None -> skip endcap instance
    model_file: str
    file_attr: str  # "ModelPath" (ONNX/Torch) or "FilePath" (HDF5)
    applicable_particles: frozenset  # e.g. frozenset({"e+","e-","gamma"})
    etrigger_gev: Mapping[str, float]  # bare floats; helper multiplies by GeV
    correct_angles: bool
    is_hadron: bool = False
    optimize_flag: Optional[int] = None
    intra_op_threads: Optional[int] = None


PRESETS: dict = {}


def _register_presets() -> None:
    from . import presets as _presets_mod

    for attr in dir(_presets_mod):
        if attr.startswith("_"):
            continue
        val = getattr(_presets_mod, attr)
        if isinstance(val, ShowerPreset):
            PRESETS[attr] = val


def add_shower_model(kernel, preset: ShowerPreset, geometry: Geometry = ILD) -> None:
    # DO NOT MOVE — GeV must resolve inside the callback so Etrigger gets the right unit factor.
    from g4units import GeV
    from DDG4 import DetectorConstruction, Geant4

    seq = Geant4(kernel).detectorConstruction()

    def _apply_common(m):
        m.Enable = True
        m.CorrectForAngles = preset.correct_angles
        m.ApplicableParticles = set(preset.applicable_particles)
        m.Etrigger = {p: e * GeV for p, e in preset.etrigger_gev.items()}
        setattr(m, preset.file_attr, preset.model_file)
        if preset.optimize_flag is not None:
            m.OptimizeFlag = preset.optimize_flag
        if preset.intra_op_threads is not None:
            m.IntraOpNumThreads = preset.intra_op_threads
        m.enableUI()

    if preset.is_hadron:
        m = DetectorConstruction(kernel, preset.barrel_plugin)
        m.isHadShower = True
        m.RegionName = "EcalBarrelRegion"  # hadron triggers in ecal, fills hcal
        m.Detector = geometry.ecal_barrel
        m.HadDetector = geometry.hcal_barrel
        m.Symmetry = geometry.ecal_barrel_symmetry
        m.HadSymmetry = geometry.hcal_barrel_symmetry
        _apply_common(m)
        seq.adopt(m)
        return

    if preset.barrel_plugin:
        m = DetectorConstruction(kernel, preset.barrel_plugin)
        m.RegionName = "EcalBarrelRegion"
        m.Detector = geometry.ecal_barrel
        m.Symmetry = geometry.ecal_barrel_symmetry
        _apply_common(m)
        seq.adopt(m)

    if preset.endcap_plugin:
        m = DetectorConstruction(kernel, preset.endcap_plugin)
        m.RegionName = "EcalEndcapRegion"
        m.Detector = geometry.ecal_endcap
        _apply_common(m)
        seq.adopt(m)


def user_physics(
    *presets: ShowerPreset, geometry: Geometry = ILD, verbose: bool = True
):
    """Return a callable compatible with SIM.physics.setupUserPhysics()."""
    if not presets:
        raise ValueError("user_physics requires at least one ShowerPreset")

    def _setup(kernel):
        from DDG4 import PhysicsList

        for p in presets:
            add_shower_model(kernel, p, geometry)
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
_CLI.add_argument("--ml-geometry", choices=sorted(_GEOMETRIES), default=None)
_CLI.add_argument(
    "--ml-config",
    default=None,
    help="JSON file: {presets:[...], geometry, overrides:{name:{...}}}.",
)


def _load_config(path: Optional[str]) -> dict:
    path = path or os.environ.get("DDML_CONFIG")
    if not path:
        return {}
    with open(path) as f:
        return json.load(f)


def _normalize_override(ov: dict) -> dict:
    ov = dict(ov)
    if "applicable_particles" in ov:
        ov["applicable_particles"] = frozenset(ov["applicable_particles"])
    return ov


def resolve_presets() -> Tuple[list, Geometry]:
    """Parse --ml-* flags out of sys.argv (leaving ddsim's args intact) and
    return the selected presets + geometry."""
    if not PRESETS:
        _register_presets()

    args, remainder = _CLI.parse_known_args()
    # Leave ddsim's own args untouched for its later parse_args().
    sys.argv[:] = [sys.argv[0]] + remainder

    cfg = _load_config(args.ml_config)
    names = args.ml_preset or cfg.get("presets") or ["CALOCLOUDS"]
    geom_name = args.ml_geometry or cfg.get("geometry") or "ILD"
    overrides = cfg.get("overrides", {})

    if geom_name not in _GEOMETRIES:
        raise SystemExit(
            f"[ml_models] unknown geometry '{geom_name}'. "
            f"Available: {sorted(_GEOMETRIES)}"
        )

    presets = []
    for name in names:
        if name not in PRESETS:
            raise SystemExit(
                f"[ml_models] unknown preset '{name}'. Available: {sorted(PRESETS)}"
            )
        p = PRESETS[name]
        if name in overrides:
            p = replace(p, **_normalize_override(overrides[name]))
        presets.append(p)
    return presets, _GEOMETRIES[geom_name]
