set(sources
  EndcapGeometry.cc
  EndcapGeometryParallelWorld.cc
  Geant4FastHitMakerGlobal.cc
  MLModelActions.cc
  PolyhedraBarrelGeometry.cc
  PolyhedraBarrelGeometryParallelWorld.cc
  RegularGridGANModel.cc
  Par04ExampleVAE.cc
  Par04CylindrialScoringMeshVAE.cc
  RegularGridBIBAEModel.cc
  RegularGridTwoAngleBIBAEModel.cc
  OctogonalBarrelTrigger.cc
  EndcapTriggerTwoAngleBIBAE.cc
  ParallelWorldCaloDiTModel.cc
  )

add_dd4hep_plugin(${PROJECT_NAME} SHARED ${sources})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${Geant4_LIBRARIES}
  EDM4HEP::edm4hep
  podio::podio
  podio::podioDict
  DD4hep::DDCore
  DD4hep::DDG4
  )
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

if(INSTRUMENT_MODEL)
  target_link_libraries(${PROJECT_NAME} PUBLIC podio::podioRootIO)
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    DDML_INSTRUMENT_MODEL_SHOWER=1
  )
endif()

if(OnnxRuntime_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE
    ONNXInference.cc
    )
  target_link_libraries(${PROJECT_NAME} PUBLIC
    OnnxRuntime
    )
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    DDML_USE_ONNX_INFERENCE
    )
endif()

if(Torch_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE
    TorchInference.cc
    )
  target_link_libraries(${PROJECT_NAME} PUBLIC
    torch
    )
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    DDML_USE_TORCH_INFERENCE
  )
endif()

if(HDF5_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE
    LoadHdf5.cc
  )
  target_link_libraries(${PROJECT_NAME} PUBLIC
    hdf5::hdf5 hdf5::hdf5_cpp MPI::MPI_CXX
  )
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    DDML_USE_LOAD_HDF5
  )
endif()

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR})

dd4hep_instantiate_package(${PROJECT_NAME})

set(headers
  ${PROJECT_SOURCE_DIR}/include/DDML/RegularGridGANModel.h
  ${PROJECT_SOURCE_DIR}/include/DDML/Par04ExampleVAE.h
  ${PROJECT_SOURCE_DIR}/include/DDML/Par04CylindrialScoringMeshVAE.h
  ${PROJECT_SOURCE_DIR}/include/DDML/RegularGridBIBAEModel.h
  ${PROJECT_SOURCE_DIR}/include/DDML/RegularGridTwoAngleBIBAEModel.h
  ${PROJECT_SOURCE_DIR}/include/DDML/PolyhedraBarrelGeometry.h
  ${PROJECT_SOURCE_DIR}/include/DDML/PolyhedraBarrelGeometryParallelWorld.h
  ${PROJECT_SOURCE_DIR}/include/DDML/ModelInterface.h
  ${PROJECT_SOURCE_DIR}/include/DDML/InferenceInterface.h
  ${PROJECT_SOURCE_DIR}/include/DDML/GeometryInterface.h
  ${PROJECT_SOURCE_DIR}/include/DDML/Geant4FastHitMakerGlobal.h
  ${PROJECT_SOURCE_DIR}/include/DDML/FastMLShower.h
  ${PROJECT_SOURCE_DIR}/include/DDML/EndcapGeometry.h
  ${PROJECT_SOURCE_DIR}/include/DDML/EndcapGeometryParallelWorld.h
  ${PROJECT_SOURCE_DIR}/include/DDML/EndcapTriggerTwoAngleBIBAE.h
  ${PROJECT_SOURCE_DIR}/include/DDML/OctogonalBarrelTrigger.h
  ${PROJECT_SOURCE_DIR}/include/DDML/TriggerInterface.h
  ${PROJECT_SOURCE_DIR}/include/DDML/DDML.h
  ${PROJECT_SOURCE_DIR}/include/DDML/ParallelWorldCaloDiTModel.h
  )

if(OnnxRuntime_FOUND)
  list(APPEND headers ${PROJECT_SOURCE_DIR}/include/DDML/ONNXInference.h)
endif()

if(Torch_FOUND)
  list(APPEND headers ${PROJECT_SOURCE_DIR}/include/DDML/TorchInference.h)
endif()

if(HDF5_FOUND)
  list(APPEND headers ${PROJECT_SOURCE_DIR}/include/DDML/LoadHdf5.h)
endif()

#install(FILES
#  ${headers}
#  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
