import sys
import os

endtoendallshowers_path = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../EndToEndAllShowers")
)
assert os.path.exists(endtoendallshowers_path)
generator_module = os.path.join(endtoendallshowers_path, "generator.py")
assert os.path.exists(generator_module)

sys.path.append(endtoendallshowers_path)
from generator import run_photon as run_inference

