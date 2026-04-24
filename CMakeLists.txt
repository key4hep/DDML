#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.15)
project(DDML LANGUAGES CXX)

# Provides install directory variables as defined for GNU software
include(GNUInstallDirs)
set(CMAKE_INSTALL_LIBDIR lib)  # Make sure that the generated thisDDML.sh script points to the right location

include(cmake/Key4hepConfig.cmake)


# Change the default install prefix away from /usr/local
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  message(STATUS "Setting install prefix to ${PROJECT_SOURCE_DIR}/install")
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install CACHE STRING "Install prefix" FORCE)
endif()

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

find_package(onnxruntime)
if(onnxruntime_FOUND)
  message("ONNX Runtime inference library found.")
else()
  message("ONNX Runtime not found!")
endif()

find_package(Torch)
if(Torch_FOUND)
  find_package( OpenMP REQUIRED)
  message("Torch inference library found.")
else()
  message("Torch not found!")
endif()

option(DDML_ENABLE_EMBEDDED_PYINFERENCE "Build embedded-Python inference backend" OFF)

if(DDML_ENABLE_EMBEDDED_PYINFERENCE)
  find_package(Python3 3.9 COMPONENTS Interpreter Development NumPy REQUIRED)
  find_package(pybind11 2.10 CONFIG REQUIRED)
  message("Embedded-Python inference backend enabled (Python ${Python3_VERSION}).")
endif()

if(NOT Torch_FOUND AND NOT onnxruntime_FOUND)
  if(NOT DDML_ENABLE_EMBEDDED_PYINFERENCE)
    message(FATAL_ERROR "Cannot find ONNX or Torch and embedded-Python inference backend is turned off. Need at least one.")
  else()
    message(STATUS "Only building embedded-Python inference backend because ONNX and Torch have not been found.")
  endif()
endif()

find_package(DD4hep REQUIRED COMPONENTS DDG4)

add_subdirectory(src)
add_subdirectory(python)

if (DOWNLOAD_MODELS)
  include(ExternalData)
  list(APPEND ExternalData_URL_TEMPLATES
    "https://key4hep.web.cern.ch:443/MLFastShowerModels/%(hash)"
  )
  add_subdirectory(models)

  ExternalData_Add_Target(model_download)
endif()

include(CTest)
if (BUILD_TESTING)
  if (NOT DOWNLOAD_MODELS)
    message(WARNING "Tests are enabled but models are not downloaded. They will likely fail")
  endif()
  add_subdirectory(tests)
endif()
