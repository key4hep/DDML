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

import os
import numpy as np
import torch

torch.set_num_threads(1)

_model_path = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../models/CC3_SF_2A.pt")
)
_model = torch.jit.load(_model_path).eval().to("cpu")


@torch.inference_mode()
def run_inference(inputs):
    # inputs is a list of 3 float32 numpy arrays: energy, theta_deg, phi_deg.
    # torch.from_numpy aliases the C++ buffer — do NOT mutate the arrays.
    tensors = [torch.from_numpy(a) for a in inputs]
    out = _model(*tensors)
    # Return a contiguous flat float32 view; the C++ side memcpy's into
    # the pre-allocated std::vector<float>.
    return out.detach().cpu().numpy().astype(np.float32, copy=False).ravel()
