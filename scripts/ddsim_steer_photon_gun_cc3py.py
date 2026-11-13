"""Minimal ddsim steering file: CaloClouds3 through DDML's embedded-Python
interface, configured entirely by the presets in python/ddml/configs.py
(CC3_BARREL_PY_INTERFACE and CC3_ENDCAP_PY_INTERFACE). Nothing from the presets
is overridden here; only the gun, event count and output are set.

Needs the ddml package and the wrapper module (cc3_sf_2a_wrapper) importable and
CC3_SF_2A.pt fetched by the build. Both are installed and set up by:

  source $DDML/install/bin/thisDDML.sh
  ddsim --steeringFile ddsim_steer_cc3_py_interface.py \
        --compactFile $k4geo_DIR/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
"""

from DDSim.DD4hepSimulation import DD4hepSimulation
from g4units import GeV

from ddml import ddml_physics
from ddml.configs import CC3_BARREL_PY_INTERFACE, CC3_ENDCAP_PY_INTERFACE

SIM = DD4hepSimulation()

SIM.enableGun = True
SIM.gun.particle = "gamma"
SIM.gun.energy = 25 * GeV
SIM.gun.distribution = "uniform"

SIM.numberOfEvents = 10
SIM.outputFile = "cc3_py_interface.edm4hep.root"

SIM.physics.setupUserPhysics(
    ddml_physics([CC3_BARREL_PY_INTERFACE, CC3_ENDCAP_PY_INTERFACE])
)
