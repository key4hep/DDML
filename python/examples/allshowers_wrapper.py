"""Reference wrapper that runs models/CC3_SF_2A.pt via torch.jit.

Serves two purposes:
  1. End-to-end example for the PyTorchEmbeddedInference backend — any
     new Python-only model follows the same shape.
  2. Verification vehicle: running this wrapper through the embedded
     backend must produce event-by-event identical output to the pure
     C++ TorchInference path on the same model.

DDML embedded-Python contract:
    run_inference(inputs: list[np.ndarray]) -> np.ndarray (flat float32)
Planned sketch
'''
import allshowers
import torch
import os

_TMP_DIR='/path/to/some/tmp'
_CNF_WEIGHTS='/other/path/to/cnf/weights.pt'
_PCFM_WEIGHTS='/other/path/to/pcfm/weights.pt'
_HARDCODED_CNF_CONFIG={"thing": "value"}  # what would be in the allshowers yaml file
_HARDCODED_PCFM_CONFIG={"thing": "other value"}  # what would be in the pointcountfm yaml file

if not have_setup_run_dirs: # possible multi-user, use lock file
  run_dir = setup_file_structure_symlinks(_TMP_DIR, _CNF_WEIGHTS, _PCFM_WEIGHTS)

generator = allshowers.unified_sampling.Generator(# args...
)

sampler = allshowers.unified_sampling.Sampler(# args...
)

generator = generator.to(device)
generator.eval()
sampler = sampler.to(device)
sampler.eval()

device = 'cuda' if torch.cuda_is_avaliable() else 'cpu'

def run_inference(energy, theta_deg, phi_deg, pdg):
   direction = direction_from_theta_phi(theta_deg, phi_deg)
   label = label_from_pdg(pdg)
   samples = generate(
            generator,
            sampler,
            [energy],
            [direction],
            [label],
            num_particles=1,
        )
    return samples[0]
'''
"""

import os
import numpy as np
import torch
import allshowers

torch.set_num_threads(1)

_model_path = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../models/CC3_SF_2A.pt")
)
_model = torch.jit.load(_model_path).eval().to("cpu")


@torch.inference_mode()
def run_inference(inputs):
    # inputs is a list of 4 float32 numpy arrays: energy, theta_deg, phi_deg, pdg
    # torch.from_numpy aliases the C++ buffer — do NOT mutate the arrays.
    tensors = [torch.from_numpy(a) for a in inputs]
    out = _model(*tensors)
    # Return a contiguous flat float32 view; the C++ side memcpy's into
    # the pre-allocated std::vector<float>.
    return out.detach().cpu().numpy().astype(np.float32, copy=False).ravel()
