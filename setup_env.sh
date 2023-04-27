#!/usr/bin/env bash

# Script to setup an envrionment that allows to use (py)torch and Marlin
# together. Based on LCG_101 and a HEAD ilcsoft release

function prepend_path() {
    local path_var=${1}
    local val=${2}

    if [ -z "${!path_var+x}" ]; then
        export ${path_var}=${val}
    else
        export ${path_var}=${val}:${!path_var}
    fi
}

# Bring in python libs from LCG_101 (this already includes torch and onnxruntime)
prepend_path PYTHONPATH /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/lib/python3.9/site-packages

# Bring in necessary torch libs
prepend_path LD_LIBRARY_PATH /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/lib/python3.9/site-packages/torch/lib
# setup cmake prefix path so that it can easily be found
prepend_path CMAKE_PREFIX_PATH /cvmfs/sft.cern.ch/lcg/releases/torch/1.9.0-38e04/x86_64-centos7-gcc11-opt/lib/python3.9/site-packages/torch/share/cmake

# Bring in onnx runtime libs
prepend_path LD_LIBRARY_PATH /cvmfs/sft.cern.ch/lcg/releases/onnxruntime/1.8.0-47224/x86_64-centos7-gcc11-opt/lib
# setup cmake prefix path so that it can easily be found
prepend_path CMAKE_PREFIX_PATH /cvmfs/sft.cern.ch/lcg/releases/onnxruntime/1.8.0-47224/x86_64-centos7-gcc11-opt

# Setup a key4hep release that should in principle work with the above libraries
source /cvmfs/ilc.desy.de/key4hep/init_ilcsoft.sh
#source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02/init_ilcsoft.sh

#source /afs/desy.de/project/ilcsoft/sw/x86_64_gcc75_ub1804/v02-02-02/init_ilcsoft.sh

#source /cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-02/init_ilcsoft.sh

#source /cvmfs/ilc.desy.de/key4hep/setup.sh

