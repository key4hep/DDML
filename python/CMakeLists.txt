install(DIRECTORY ml_models
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages
        FILES_MATCHING PATTERN "*.py"
        PATTERN "__pycache__" EXCLUDE)
