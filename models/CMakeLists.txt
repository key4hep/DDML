# The models to download
set(models
  BIBAE_Full_PP_cut.pt
  BIBAE_Two_Angle_Full_PP_cut.pt
  CC3_SF_2A.pt
  CC3_paper_checkpoint.pt
  #L2LFlowsx9.pt

  # AllShowers in three parts
  AllShowers_CNF_v1.pt
  AllShowers_pcfm_v1.pt
  AllShowers_trafos_v1.pt
)

set(models_data)
foreach(model ${models})
  list(APPEND models_data "DATA{${model}}")
endforeach()

ExternalData_Expand_Arguments(model_download
    download_paths
    ${models_data}
)

# Install the actual files (not the symlinks into the build tree), so that the
# installation is self-contained. ddml.paths finds them there at runtime.
foreach(path ${download_paths})
  get_filename_component(model_name ${path} NAME)
  install(CODE "
    get_filename_component(_ddml_model_file \"${path}\" REALPATH)
    file(INSTALL
      DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${DDML_MODEL_INSTALL_DIR}\"
      TYPE FILE
      RENAME \"${model_name}\"
      FILES \"\${_ddml_model_file}\"
    )
  ")
endforeach()
