#!/usr/bin/env bash

# Script to setup key4hep nightly envrionment that allows to use (py)torch, onnx and Marlin
# together with DD4hep.
#
#

source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh

#source /nfs/dust/ilc/user/mckeownp/DDML_Environment/setup.sh

#source /cvmfs/ilc.desy.de/key4hep/releases/2023-05-23/key4hep-stack/2023-05-24/x86_64-centos7-gcc12.3.0-opt/7emhu/setup.sh

export DD4HEP_DIR=/nfs/dust/ilc/user/mckeownp/DDML_Environment/DD4hep/install
export PATH=$DD4HEP_DIR/bin:$PATH
export CMAKE_PREFIX_PATH=$DD4HEP_DIR:$CMAKE_PREFIX_PATH
export LD_LIBRARY_PATH=$DD4HEP_DIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$DD4HEP_DIR/lib/python3.10/site-packages:$PYTHONPATH

unset DD4hep_ROOT
unset DD4hep_DIR
export DD4HEP=$DD4HEP_DIR/examples

source /nfs/dust/ilc/user/mckeownp/DDML_Environment/k4geo/install/bin/thislcgeo.sh

export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/cvmfs/sw-nightlies.hsf.org/key4hep/releases/2023-06-24/x86_64-centos7-gcc12.2.0-opt/geant4/11.1.2-zmpvdw/lib64/

TORCH_PATH=$(dirname $(python -c 'import torch; print(f"{torch.__file__}")'))
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${TORCH_PATH}/share/cmake
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${TORCH_PATH}/lib

ONNXRUNTIME_PATH=$(dirname $(python -c 'import onnxruntime; print(f"{onnxruntime.__file__}")'))
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$(realpath ${ONNXRUNTIME_PATH}/../../../../)/lib64

export LD_LIBRARY_PATH=/nfs/dust/ilc/user/mckeownp/DDFastML/ddfastshowerml/install/lib64:$LD_LIBRARY_PATH
