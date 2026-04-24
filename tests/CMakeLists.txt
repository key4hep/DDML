function(set_test_env _test)
  set(_test_env
    LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/src:$<$<TARGET_EXISTS:torch>:$<TARGET_FILE_DIR:torch>>:$<$<TARGET_EXISTS:OnnxRuntime>:$<TARGET_FILE_DIR:OnnxRuntime>>:$<$<TARGET_EXISTS:hdf5::hdf5>:$<TARGET_FILE_DIR:hdf5::hdf5>>:$ENV{LD_LIBRARY_PATH}
    MODEL_DIR=${PROJECT_BINARY_DIR}/models
    PYTHONPATH=${PROJECT_SOURCE_DIR}/python:$ENV{PYTHONPATH}
  )

  set_property(TEST ${_test} PROPERTY ENVIRONMENT "${_test_env}")
endfunction()

add_test(NAME run_cc3_ild COMMAND
  ddsim --steeringFile ${PROJECT_SOURCE_DIR}/scripts/ddsim_steer.py --compactFile $ENV{k4geo_DIR}/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml --ml-preset CC3_BARREL --ml-preset CC3_ENDCAP
)

set_test_env(run_cc3_ild)
set_tests_properties(
  run_cc3_ild

  PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/scripts
)
