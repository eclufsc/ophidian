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
target_link_libraries(ophidian_tests
    PRIVATE Catch
    PRIVATE ophidian_circuit
    PRIVATE ophidian_design
    PRIVATE ophidian_entity_system
    PRIVATE ophidian_floorplan
    PRIVATE ophidian_geometry
    PRIVATE ophidian_interconnection
    PRIVATE ophidian_parser
    PRIVATE ophidian_placement
    PRIVATE ophidian_standard_cell
)

if(OPHIDIAN_TESTS_FULLY_STATIC)
    # Link EVERYTHING staticly!
    set_target_properties(ophidian_tests PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++ -pthread"
    )
endif()

################################################################################
# Prepare test input environment. 
################################################################################

message("Creating symlink for test files")
execute_process(
    COMMAND ln -f -s ${PROJECT_SOURCE_DIR}/test/input_files ${PROJECT_BINARY_DIR}/test/.
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
add_test(NAME unit_test COMMAND ophidian_tests)
