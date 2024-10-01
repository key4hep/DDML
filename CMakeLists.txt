#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.15)
project(DDML)

# Provides install directory variables as defined for GNU software
include(GNUInstallDirs)

# Define a default build type can be overriden by passing
# ``-DCMAKE_BUILD_TYPE=<type>`` when invoking CMake
if(NOT CMAKE_CONFIGURATION_TYPES)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  else()
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}"
      CACHE STRING "Choose the type of build, options are: None Release MinSizeRel Debug RelWithDebInfo"
      FORCE
      )
  endif()
endif()

# Change the default install prefix away from /usr/local
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS "Setting install prefix to ${PROJECT_SOURCE_DIR}/install")
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install CACHE STRING "Install prefix" FORCE)
endif()

# Set up C++ Standard
# ``-DCMAKE_CXX_STANDARD=<standard>`` when invoking CMake
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")

if(NOT CMAKE_CXX_STANDARD MATCHES "20|23")
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}, supported values are 20 and 23")
endif()

# Prevent CMake falls back to the latest standard the compiler does support
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disables the use of compiler-specific extensions, hence makes sure the code
# works for a wider range of compilers
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

option(INSTRUMENT_MODEL "Instrument the steps of the showerModel call" OFF)

option(DOWNLOAD_MODELS "Download and install the models that are stored externally" ON)

#---------------------------------------------------
# external packages

find_package(EDM4HEP REQUIRED)

# make sure the onnx find script is found:
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# HDF5, MPI headers are also required
# HDF5 seems not to properly export targets, so have to find MPI explicitly
find_package(HDF5 COMPONENTS C CXX)
find_package(MPI REQUIRED)
if(HDF5_FOUND)
  message("HDF5 found")
  message(STATUS "INCLUDE LOCATION" ${HDF5_INCLUDE_DIRS})
  message(STATUS "version" ${HDF5_VERSION})
  message(STATUS "DEFINITIONS" ${HDF5_DEFINITIONS})
  message(STATUS "LIBRARIES" ${HDF5_LIBRARIES})
  message(STATUS "HL_LIBRARIES" ${HDF5_HL_LIBRARIES})
else()
  message("HDF5 not found!")
endif()
#--------------------------------------------------

find_package(OnnxRuntime)
if(OnnxRuntime_FOUND)
  message("ONNX Runtime inference library found.")
else()
  message("ONNX Runtime not found!")
endif()

find_package(Torch)
if(Torch_FOUND)
  message("Torch inference library found.")
else()
  message("Torch not found!")
endif()

if(NOT Torch_FOUND AND NOT OnnxRuntime_FOUND)
  message(FATAL "Cannot find ONNX or Torch for ML inference. Need at least one")
endif()

find_package(DD4hep REQUIRED COMPONENTS DDG4)
dd4hep_set_compiler_flags()
dd4hep_use_python_executable()

add_subdirectory(src)

if (DOWNLOAD_MODELS)
  include(ExternalData)
  list(APPEND ExternalData_URL_TEMPLATES
    "https://key4hep.web.cern.ch:443/MLFastShowerModels/%(hash)"
  )
  add_subdirectory(models)

  ExternalData_Add_Target(model_download)
endif()
