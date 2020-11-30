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

file(GLOB_RECURSE ophidian_experiments_test
    "*.h"
    "*.cpp"
)

# Add test target
add_executable(run_tests ${ophidian_experiments_test})


set(LIBS 
    PRIVATE Catch
    PRIVATE ophidian_circuit_static
    PRIVATE ophidian_design_static
    PRIVATE ophidian_entity_system_static
    PRIVATE ophidian_floorplan_static
    PRIVATE ophidian_geometry_static
    PRIVATE ophidian_interconnection_static
    PRIVATE ophidian_routing_static
    PRIVATE ophidian_placement_static
    PRIVATE ophidian_parser_static
    PRIVATE ophidian_util_static
    PRIVATE dl
)

# Link target dependencies
target_link_libraries(run_tests ${LIBS})


if(OPHIDIAN_EXPERIMENTS_FULLY_STATIC)
    # Link EVERYTHING staticly!
    set_target_properties(run_tests PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++ -pthread"
    )
endif()

################################################################################
# Prepare test input environment. 
################################################################################

message("Creating symlink for test files")
execute_process(
    COMMAND ln -f -s ${PROJECT_SOURCE_DIR}/test/input_files ${PROJECT_BINARY_DIR}/experiments/.
    RESULT_VARIABLE result
)

################################################################################
# Add test to ctest
################################################################################

enable_testing()
add_test(NAME unit_test COMMAND run_tests)

