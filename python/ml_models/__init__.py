from typing import List
import argparse
import sys

from .common import add_shower_model, user_physics, ModelPreset
from . import presets


PRESETS: dict = {}


def _register_presets() -> None:
    from . import presets as _presets_mod

    for attr in dir(_presets_mod):
        if attr.startswith("_"):
            continue
        val = getattr(_presets_mod, attr)
        if isinstance(val, ModelPreset):
            PRESETS[attr] = val


_register_presets()


def get_presets_from_args() -> List[ModelPreset]:
    # Add a single argument (in a way that doesn't interfere with the arg parsing of
    # ddsim). We do this to make it possible to dynamically get preests from CLI
    _cli = argparse.ArgumentParser(add_help=False)
    _cli.add_argument(
        "--ml-preset",
        action="append",
        default=None,
        help="Preset dotted name (e.g. torch.CALOCLOUDS). Repeat to compose.",
    )

    # Make sure to leave all other arguments untouched. Only remove ours
    args, remainder = _cli.parse_known_args()
    sys.argv[:] = [sys.argv[0]] + remainder

    presets = []
    for preset in args.ml_preset:
        presets.append(PRESETS[preset])
    return presets


__all__ = [
    "ModelPreset",
    "PRESETS",
    "add_shower_model",
    "user_physics",
    "presets",
    "get_presets_from_args",
]
