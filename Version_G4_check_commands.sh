#!/bin/bash

## commands to be run in singularity container

source setup_env_stable.sh;

### check build et al. are up to date and additional source
#cd build;
#cmake ..;
#make -j$(nproc) install;
source install/bin/thisDDML.sh;
cd ./scripts/;

# Regular detector model
# run the simulation
#ddsim --steeringFile ddsim_steer_Version_G4_check.py --compactFile ../Regular_detector/ILDConfig/StandardConfig/production/lcgeo/v00-16-06/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml;

# run the simulation
ddsim --steeringFile ddsim_steer_Version_G4_check.py --compactFile $lcgeo_DIR/ILD/compact/ILD_l5_o1_v02/ILD_l5_o1_v02.xml;