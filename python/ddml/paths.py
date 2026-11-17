"""Locate the (externally stored) model files that are fetched by the build."""

import os
from pathlib import Path


def model_dir() -> Path:
    """Get the directory containing the model files

    In order of precedence this is
    - the DDML_MODEL_DIR environment variable (if set)
    - the models directory of the installation (if ddml is used from there)
    - the models directory of the source tree (if ddml is used from there)
    """
    env_dir = os.environ.get("DDML_MODEL_DIR")
    if env_dir:
        return Path(env_dir)

    try:
        from ._install_paths import MODEL_DIR

        return MODEL_DIR
    except ImportError:
        pass

    return Path(__file__).resolve().parent.parent.parent / "models"


def model_path(name: str) -> str:
    """Get the full path to the model file with the given name"""
    return str(model_dir() / name)
