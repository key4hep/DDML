#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.15)
project(DDML)

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

#---------------------------------------------------
# external packages

find_package(EDM4HEP REQUIRED)

# make sure the onnx find script is found:
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OnnxRuntime REQUIRED)
if(OnnxRuntime_FOUND)
  message("ONNX Runtime inference library found.")
  add_definitions(-DUSE_INFERENCE)
  add_definitions(-DUSE_INFERENCE_ONNX)
else()
  message("ONNX Runtime not found!")
endif()

#----------------------------------------------------
IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
dd4hep_use_python_executable()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
		    ${OnnxRuntime_INCLUDE_DIR}
		    ${DD4hep_INCLUDE_DIRS})


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
add_dd4hep_plugin(${PROJECT_NAME} SHARED ${sources})

target_link_libraries(${PROJECT_NAME}
  ${Geant4_LIBRARIES}
  ${OnnxRuntime_LIBRARY}
  EDM4HEP::edm4hep
  EDM4HEP::edm4hepDict
  podio::podio
  podio::podioRootIO
  podio::podioDict
  DD4hep::DDCore
  DD4hep::DDG4
  )


# Make sure to use the correct typedef depending on which version of podio we
# build a against
if(${podio_VERSION} VERSION_GREATER_EQUAL "0.14.1")
  target_compile_definitions(${PROJECT_NAME} PUBLIC use_new_EDM4hep=1)
else()
  target_compile_definitions(SimpleCalo PUBLIC use_new_EDM4hep=0)
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build gflash. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
### set(gflash_SCRIPTS
###     test.mac vis.mac
###   )
### 
### foreach(_script ${gflash_SCRIPTS})
###   configure_file(
###     ${PROJECT_SOURCE_DIR}/${_script}
###     ${PROJECT_BINARY_DIR}/${_script}
###     COPYONLY
###     )
### endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
#add_custom_target(simplecalo DEPENDS SimpleCalo)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})


install(TARGETS ${PROJECT_NAME} DESTINATION lib)


#Create this_package.sh file, and install
#dd4hep_instantiate_package(SimpleCalo)
