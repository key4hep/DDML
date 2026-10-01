from typing import List, Tuple
import argparse
import sys

from .setup_physics import add_shower_model, ddml_physics
from .model import ModelConfig


_PRESETS: dict = {}


def _register_presets() -> None:
    """Make all known configurations available for easy retrieval via command
    line arguments"""
    from . import configs as _config_mod

    for attr in dir(_config_mod):
        if attr.startswith("_"):
            continue
        val = getattr(_config_mod, attr)
        if isinstance(val, ModelConfig):
            _PRESETS[attr] = val


_register_presets()


def get_fastsim_configuration() -> Tuple[List[ModelConfig], bool]:
    """Get the fast simulation configuration from the commandline arguments

    Returns the list of models to be configured in the physics list and a
    boolean value for steering whether the recording of information at the
    calorimeter entry should be turned on
    """
    # Add DDML specific arguments (in a way that doesn't interfere with the arg
    # parsing of ddsim). We do this to make it possible to dynamically get
    # preests from CLI
    _cli = argparse.ArgumentParser(add_help=False)
    _cli.add_argument(
        "--ml-model",
        action="append",
        default=None,
        help="Preset dotted name (e.g. torch.CALOCLOUDS). Repeat to compose.",
    )
    _cli.add_argument(
        "--record-calo-entry",
        action="store_true",
        default=False,
        help="Record information about particles as they enter the calorimeter",
    )

    # Make sure to leave all other arguments untouched. Only remove ours
    args, remainder = _cli.parse_known_args()
    sys.argv[:] = [sys.argv[0]] + remainder

    presets = []
    for preset in args.ml_model:
        presets.append(_PRESETS[preset])

    return presets, args.record_calo_entry


__all__ = [
    "ModelConfig",
    "add_shower_model",
    "ddml_physics",
    "get_fastsim_configuration",
]
