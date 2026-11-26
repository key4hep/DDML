set(DDML_PYTHON_INSTALL_DIR
  ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
)

install(DIRECTORY ddml
        DESTINATION ${DDML_PYTHON_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.py"
        PATTERN "__pycache__" EXCLUDE)

# The wrappers for the embedded python inference are imported as top level
# modules (see the PythonModule plugin properties in ddml/configs.py)
install(FILES
  examples/allshowers_wrapper.py
  examples/cc3_sf_2a_wrapper.py
  DESTINATION ${DDML_PYTHON_INSTALL_DIR}
)

# Tell the installed ddml package where to find the installed models. Use a
# relative path to keep the installation relocatable
file(RELATIVE_PATH DDML_MODEL_DIR_REL
  ${CMAKE_INSTALL_PREFIX}/${DDML_PYTHON_INSTALL_DIR}/ddml
  ${CMAKE_INSTALL_PREFIX}/${DDML_MODEL_INSTALL_DIR}
)
configure_file(ddml/_install_paths.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/ddml/_install_paths.py
  @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ddml/_install_paths.py
  DESTINATION ${DDML_PYTHON_INSTALL_DIR}/ddml
)
