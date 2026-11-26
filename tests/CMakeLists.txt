function(set_test_env _test)
  set(_test_env
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<$<TARGET_EXISTS:torch>:$<TARGET_FILE_DIR:torch>>:$<$<TARGET_EXISTS:OnnxRuntime>:$<TARGET_FILE_DIR:OnnxRuntime>>:$<$<TARGET_EXISTS:hdf5::hdf5>:$<TARGET_FILE_DIR:hdf5::hdf5>>:$ENV{LD_LIBRARY_PATH}
    DDML_MODEL_DIR=${PROJECT_BINARY_DIR}/models
    PYTHONPATH=${PROJECT_SOURCE_DIR}/python:${PROJECT_SOURCE_DIR}/python/examples:$ENV{PYTHONPATH}
  )

  set_property(TEST ${_test} PROPERTY ENVIRONMENT "${_test_env}")
endfunction()

# Collect the checks that check_test_outputs.py should run and the fixtures
# producing their inputs, depending on which backends are built
set(_output_checks)
set(_output_fixtures)

if (Torch_FOUND)
  add_test(NAME run_cc3_ild COMMAND
    ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py
          --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
          --ml-model CC3_BARREL --ml-model CC3_ENDCAP
          --record-calo-entry
          -N 10 -G --gun.multiplicity 1 --gun.distribution=uniform
          --outputFile cc3_ild_pgun.edm4hep.root
  )
  set_test_env(run_cc3_ild)
  set_tests_properties(run_cc3_ild PROPERTIES
    FIXTURES_SETUP cc3_ild_output
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
  )

  add_test(NAME run_bibae_ild COMMAND
    ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py
          --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
          --ml-model BIBAE_BARREL --ml-model BIBAE_ENDCAP
          -N 10 -G --gun.multiplicity 1 --gun.distribution=uniform
          --outputFile bibae_ild_pgun.edm4hep.root
  )
  set_test_env(run_bibae_ild)
  set_tests_properties(run_bibae_ild PROPERTIES
    FIXTURES_SETUP bibae_ild_output
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
  )

  list(APPEND _output_checks cc3 bibae)
  list(APPEND _output_fixtures cc3_ild_output bibae_ild_output)
endif()

if (DDML_ENABLE_EMBEDDED_PYINFERENCE)
  add_test(NAME run_cc3_py_interface_ild COMMAND
    ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py
          --compactFile  $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
          -N 10 -G --gun.multiplicity 1 --gun.distribution=uniform
          --outputFile cc3_py_interface_ild_pgun.edm4hep.root
          --ml-model CC3_BARREL_PY_INTERFACE --ml-model CC3_ENDCAP_PY_INTERFACE
  )
  set_test_env(run_cc3_py_interface_ild)
  set_tests_properties(run_cc3_py_interface_ild PROPERTIES
    FIXTURES_SETUP cc3_py_interface_output
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
  )

  list(APPEND _output_checks cc3_py_interface)
  list(APPEND _output_fixtures cc3_py_interface_output)


  set(_allshowers_setup
    "source ${PROJECT_SOURCE_DIR}/scripts/setup_allshowers.sh ${PROJECT_BINARY_DIR}/allshowers_libs"
  )
  add_test(NAME setup_allshowers COMMAND bash -c "${_allshowers_setup}")
  set_tests_properties(setup_allshowers PROPERTIES
    FIXTURES_SETUP allshowers_env
  )

  add_test(NAME create_allshowers_input COMMAND
    python3 ${PROJECT_SOURCE_DIR}/tests/create_allshowers_input.py
    ${PROJECT_BINARY_DIR}/allshowers_species_input.edm4hep.root
  )
  set_tests_properties(create_allshowers_input PROPERTIES
    FIXTURES_SETUP allshowers_input
  )

  add_test(NAME run_allshowers_ild COMMAND bash -c
    "${_allshowers_setup} && \
       ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py \
             --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml \
             --inputFiles ${PROJECT_BINARY_DIR}/allshowers_species_input.edm4hep.root \
             -N -1 \
             --outputFile allshowers_ild_species.edm4hep.root \
             --ml-model AS_BARREL_PY_INTERFACE --ml-model AS_ENDCAP_PY_INTERFACE"
  )
  set_test_env(run_allshowers_ild)
  set_tests_properties(run_allshowers_ild PROPERTIES
    FIXTURES_REQUIRED "allshowers_env;allshowers_input"
    FIXTURES_SETUP allshowers_output
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
  )

  list(APPEND _output_checks allshowers)
  list(APPEND _output_fixtures allshowers_output)
endif()

if (_output_checks)
  add_test(NAME check_test_outputs COMMAND
    python3 ${PROJECT_SOURCE_DIR}/tests/check_test_outputs.py ${_output_checks}
  )
  set_tests_properties(check_test_outputs PROPERTIES
    FIXTURES_REQUIRED "${_output_fixtures}"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
  )
endif()
