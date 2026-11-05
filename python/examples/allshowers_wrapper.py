"""Reference wrapper that runs models/CC3_SF_2A.pt via torch.jit.

Serves two purposes:
  1. End-to-end example for the PyTorchEmbeddedInference backend — any
     new Python-only model follows the same shape.
  2. Verification vehicle: running this wrapper through the embedded
     backend must produce event-by-event identical output to the pure
     C++ TorchInference path on the same model.

DDML embedded-Python contract:
    run_inference(inputs: list[np.ndarray]) -> np.ndarray (flat float32)
"""
import math
import os
import tempfile
from pathlib import Path
import numpy as np

import torch
import yaml
from allshowers.unified_sampling import Generator, generate, init_sampler
import allshowers.flow_matching
import allshowers.transformer
from torch.nn.attention.flex_attention import create_block_mask

# unified_sampling compiles all of compute_mask (fullgraph=True); torch 2.9's
# Dynamo can't trace the nested mask_fn it passes to create_block_mask. Do what
# AllShowers main does instead: compute_mask uncompiled, create_block_mask compiled.
allshowers.transformer.create_block_mask = torch.compile(create_block_mask)
allshowers.flow_matching.compute_mask = allshowers.transformer.compute_mask.__wrapped__

torch.set_num_threads(1)

_TMP_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../models/tmp"))
if not os.path.exists(_TMP_DIR):
    os.mkdir(_TMP_DIR)
_CNF_WEIGHTS = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../models/AllShowers_CNF_v1.pt")
)
_CNF_TRAFOS = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../models/AllShowers_trafos_v1.pt")
)
_PCFM_WEIGHTS = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../models/AllShowers_pcfm_v1.pt")
)

# The parts of the CNF run's conf.yaml that Generator reads (repo defaults from
# conf/transformer.yaml). Must match the run the weights came from.
_HARDCODED_CNF_CONFIG = {
    "data": {
        "samples_energy_trafo": [
            ["Log", {"alpha": 0.0}],
            ["StandardScaler", {"shape": [1, 1]}],
        ],
        "samples_coordinate_trafo": [["StandardScaler", {"shape": [1, 1, 2]}]],
        "cond_trafo": [
            ["Log", {"alpha": 0.0}],
            ["StandardScaler", {"shape": [1, 1]}],
        ],
    },
    "model": {
        "num_layers": 78,
        "dim_inputs": [3, 6, 4],
        "dim_embedding": 64,
        "num_head": 4,
        "num_blocks": 4,
        "dim_feedforward": 256,
        "num_points_cond": 512,
        "activation": "GELU",
        "num_layer_cond": 4,
        "num_particles": 12,
        "flow_config": {"frequencies": 3},
    },
}

# The parts of the PointCountFM run's conf.yaml that init_sampler reads (repo
# defaults from config/config.yaml). Must match the run the weights came from.
_HARDCODED_PCFM_CONFIG = {
    "data": {
        "transform_num_points": [
            ["Log", {"alpha": 0.5}],
            ["StandardScaler", {"shape": [1, 78]}],
        ],
        "transform_inc": [["StandardScaler", {"shape": [1, 1]}]],
    },
    "model": {
        "name": "FullyConnected",
        "dim_input": 78,
        "dim_condition": 16,
        "dim_time": 6,
        "hidden_dims": [128, 256, 512, 256, 128],
    },
}

# Label order both models were trained with (label = index in this list).
_PDG_LIST = [11, -11, 22, 130, 211, -211, 321, -321, 2112, -2112, 2212, -2212]

device = "cuda" if torch.cuda.is_available() else "cpu"


def _make_run_dir(run_dir: Path, config: dict, links: dict[str, str]) -> Path:
    for name, target in links.items():
        (run_dir / name).parent.mkdir(parents=True, exist_ok=True)
        (run_dir / name).symlink_to(os.path.abspath(target))
    (run_dir / "conf.yaml").write_text(yaml.safe_dump(config))
    return run_dir


# Both loaders read every file in their constructor, so the run dirs only need
# to exist while loading. A private temp dir per process leaves nothing to lock.
with tempfile.TemporaryDirectory(dir=_TMP_DIR) as tmp:
    cnf_dir = _make_run_dir(
        Path(tmp, "cnf"),
        _HARDCODED_CNF_CONFIG,
        {"weights/best.pt": _CNF_WEIGHTS, "preprocessing/trafos.pt": _CNF_TRAFOS},
    )
    pcfm_dir = _make_run_dir(
        Path(tmp, "pcfm"), _HARDCODED_PCFM_CONFIG, {"model.pt": _PCFM_WEIGHTS}
    )
    # same settings as the unified_sampling.py defaults
    generator = Generator(
        cnf_dir, num_timesteps=16, solver="midpoint", resize_factor=1.033
    )
    sampler = init_sampler(pcfm_dir, steps=50)

generator = generator.to(device).eval()
sampler = sampler.to(device).eval()


@torch.inference_mode()
def run_inference(inputs):
    # inputs is a list of 4 float32 numpy arrays: energy, theta_deg, phi_deg, pdg
    # torch.from_numpy aliases the C++ buffer — do NOT mutate the arrays.
    energy, theta_deg, phi_deg, pdg = (a.item() for a in inputs)
    theta, phi = math.radians(theta_deg), math.radians(phi_deg)
    direction = [
        math.sin(theta) * math.cos(phi),
        math.sin(theta) * math.sin(phi),
        math.cos(theta),
    ]
    samples = generate(
        generator,
        sampler,
        torch.tensor([[energy]], dtype=torch.float32, device=device),
        torch.tensor([direction], dtype=torch.float32, device=device),
        torch.tensor([_PDG_LIST.index(round(pdg))], device=device),
        num_particles=len(_PDG_LIST),
    )
    sample = samples[0][samples[0, :, 3] > 0]
    sample = sample.detach().cpu().numpy().astype(np.float32, copy=False)
    layer = sample[:, 2].astype(int)
    per_layer = np.bincount(
        layer, minlength=_HARDCODED_CNF_CONFIG["model"]["num_layers"]
    ).astype(np.float32)
    out = np.concatenate([per_layer, sample.ravel()])
    return out
