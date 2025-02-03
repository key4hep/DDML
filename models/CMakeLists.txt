# The models to download
set(models
  BIBAE_Full_PP_cut.pt
  CC3_SF_2A.pt
  #L2LFlowsx9.pt
)

set(models_data)
foreach(model ${models})
  list(APPEND models_data "DATA{${model}}")
endforeach()

ExternalData_Expand_Arguments(model_download
    download_paths
    ${models_data}
)

# Link them back to model directory
add_custom_target(copy_models_to_source ALL
  COMMENT "Copying models (symlinking) to source directory"
  DEPENDS model_download
)

foreach(path ${download_paths})
  add_custom_command(
    TARGET copy_models_to_source
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${path} ${PROJECT_SOURCE_DIR}/models
  )
endforeach()

install(FILES ${models} DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/models)
