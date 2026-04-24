function(set_test_env _test)
  set(_test_env
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<$<TARGET_EXISTS:torch>:$<TARGET_FILE_DIR:torch>>:$<$<TARGET_EXISTS:OnnxRuntime>:$<TARGET_FILE_DIR:OnnxRuntime>>:$<$<TARGET_EXISTS:hdf5::hdf5>:$<TARGET_FILE_DIR:hdf5::hdf5>>:$ENV{LD_LIBRARY_PATH}
    MODEL_DIR=${PROJECT_BINARY_DIR}/models
    PYTHONPATH=${PROJECT_SOURCE_DIR}/python:${PROJECT_SOURCE_DIR}/python/examples:$ENV{PYTHONPATH}
  )

  set_property(TEST ${_test} PROPERTY ENVIRONMENT "${_test_env}")
endfunction()

add_test(NAME run_cc3_ild COMMAND
  ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py
        --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
        --ml-model CC3_BARREL --ml-model CC3_ENDCAP
        --record-calo-entry
        -N 10 -G --gun.multiplicity 1 --gun.distribution=uniform
        --outputFile cc3_ild_pgun.edm4hep.root
)
set_test_env(run_cc3_ild)
set_tests_properties(run_cc3_ild PROPERTIES FIXTURES_SETUP cc3_ild_output)

add_test(NAME run_bibae_ild COMMAND
  ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py
        --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml
        --ml-model BIBAE_BARREL --ml-model BIBAE_ENDCAP
        -N 10 -G --gun.multiplicity 1 --gun.distribution=uniform
        --outputFile bibae_ild_pgun.edm4hep.root
)
set_test_env(run_bibae_ild)
set_tests_properties(run_bibae_ild PROPERTIES FIXTURES_SETUP bibae_ild_output)

add_test(NAME check_test_outputs COMMAND
  python3 ${PROJECT_SOURCE_DIR}/tests/check_test_outputs.py
)
set_tests_properties(check_test_outputs PROPERTIES
  FIXTURES_REQUIRED "cc3_ild_output;bibae_ild_output"
)
set_test_env(run_cc3_ild)

add_test(NAME run_cc3_pywrapper_ild COMMAND
  ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py --compactFile  $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml --ml-model CC3_BARREL_PY_INTERFACE --outputFile cc3_barre_py_interface.edm4hep.root
)
set_test_env(run_cc3_pywrapper_ild)

set_tests_properties(
  run_cc3_ild
  run_bibae_ild
  check_test_outputs
  run_cc3_pywrapper_ild

  PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
)

