#!/bin/bash
#SBATCH --time 1-00:00:00
#SBATCH --nodes 1
#SBATCH --partition maxgpu
#SBATCH --job-name main
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=henry.day-hall@desy.de
#SBATCH --output /data/dust/user/dayhallh/data/AllShowers/EndToEnd/joblogs/%j.out      # terminal output
#SBATCH --error /data/dust/user/dayhallh/data/AllShowers/EndToEnd/joblogs/%j.err

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR
source setup_env_nightly.sh
source install/bin/thisDDML.sh
ddml_python=$(readlink -f python)
plugin_python=$(readlink -f python/examples)
export PYTHONPATH=${PYTHONPATH}:${ddml_python}:${plugin_python}

cd scripts
# NOTE - be sure to change --compactFile and --inputFile apropriately
ddsim --steeringFile ddsim_steer.py \
 --compactFile $k4geo_DIR/ILD/compact/ILD_l5_o1_v02/ILD_l5_o1_v02.xml \
 --ml-model AS1_BARREL_PY_INTERFACE \
 --inputFile /data/dust/group/ilc/sft-ml/datasets/angular/simulation_inputs/ILD-barrelSmallSegment-singleParticles-gen-E1010pdg22.slcio \
 --numberOfEvents 10
 #--outputFile /data/dust/user/dayhallh/data/AllShowers/EndToEnd/test.edm4hep.root
