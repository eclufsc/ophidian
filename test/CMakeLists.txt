################################################################################
# This is the CMakeLists file for the Ophidian library Test binary. 
#
# Its main goals are:
#   - Fetch test files.
#   - Configure Catch test library.
#   - Add test target.
#   - Link test target.
#   - Prepare test input environment.
################################################################################

# Fetch test files recursevely
file(GLOB_RECURSE ophidian_run_tests_source
    "*.cpp"
)

# Add test target
add_executable(ophidian_tests ${ophidian_run_tests_source})

# Link target dependencies
target_link_libraries(ophidian_tests PUBLIC 
    Catch 
    ophidian_circuit 
    ophidian_design 
    ophidian_entity_system 
    ophidian_floorplan 
    ophidian_geometry 
    ophidian_interconnection 
    ophidian_parser 
    ophidian_placement 
    ophidian_standard_cell 
    ophidian_timing 
)

install(TARGETS ophidian_tests DESTINATION ${ophidian_install_bin_dir})

################################################################################
# Prepare test input environment. 
################################################################################

message("Creating symlink for test files")
execute_process(
    COMMAND ln -f -s ${PROJECT_SOURCE_DIR}/test/input_files ${PROJECT_BINARY_DIR}/test/.
    RESULT_VARIABLE result
)
execute_process(
    COMMAND tar -zxvf superblue18.tar.gz
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/test/input_files
    RESULT_VARIABLE result
)

message("Creating symlink FLUTE LUTs")
execute_process(
    COMMAND ln -f -s ${PROJECT_SOURCE_DIR}/3rdparty/Flute/PORT9.dat ${PROJECT_BINARY_DIR}/test/.
    RESULT_VARIABLE result
)
execute_process(
    COMMAND ln -f -s ${PROJECT_SOURCE_DIR}/3rdparty/Flute/POWV9.dat ${PROJECT_BINARY_DIR}/test/.
    RESULT_VARIABLE result
)

################################################################################
# Add test to ctest
################################################################################

enable_testing()
add_test(NAME unit_test COMMAND ophidian_tests "~[Profiling] ~[superblue18obs]")
