"""Minimal ddsim steering file: AllShowers through DDML's embedded-Python
interface, configured entirely by the presets in python/ddml/configs.py
(AS_BARREL_PY_INTERFACE and AS_ENDCAP_PY_INTERFACE). Nothing from the presets
is overridden here; only the gun, event count and output are set.

Needs the ddml package and the wrapper module (allshowers_wrapper) importable,
as DDML's own tests set them up, and models/AllShowers*.pt fetched by the build:

  source $DDML/install/bin/thisDDML.sh
  export PYTHONPATH=$DDML/python:$DDML/python/examples:$PYTHONPATH
  ddsim --steeringFile ddsim_steer_cc3_py_interface.py \
        --compactFile $k4geo_DIR/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
"""

from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import GeV

from ddml import ddml_physics
from ddml.configs import AS_BARREL_PY_INTERFACE, AS_ENDCAP_PY_INTERFACE

SIM = DD4hepSimulation()

SIM.enableGun = True
SIM.gun.particle = "gamma"
SIM.gun.energy = 25 * GeV
SIM.gun.distribution = "uniform"

SIM.numberOfEvents = 10
SIM.outputFile = "as_py_interface.edm4hep.root"

SIM.physics.setupUserPhysics(
    ddml_physics([AS_BARREL_PY_INTERFACE, AS_ENDCAP_PY_INTERFACE])
)
