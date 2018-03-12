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
file(GLOB_RECURSE ophidian_tests_source
    "*.cpp"
    "*.h"
)

if(UNCRUSTIFY_IT)
    include(uncrustify_helper)
    uncrustify_it(${ophidian_uncrustify_config} ${ophidian_tests_source})
endif()

if(RUN_UNCRUSTIFY_CHECK)
    include(uncrustify_helper)
    uncrustify_check(${ophidian_uncrustify_config} ${ophidian_tests_source})
endif()

# Add test target
add_executable(ophidian_tests ${ophidian_tests_source})

# Link target dependencies
target_link_libraries(ophidian_tests
    PRIVATE Catch
    PRIVATE ophidian_circuit_static
    PRIVATE ophidian_design_static
    PRIVATE ophidian_entity_system_static
    PRIVATE ophidian_floorplan_static
    PRIVATE ophidian_geometry_static
    PRIVATE ophidian_interconnection_static
    PRIVATE ophidian_parser_static
    PRIVATE ophidian_placement_static
    PRIVATE ophidian_standard_cell_static
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

find_file(POWV9 "POWV9.dat" PATHS 
    "/usr/share/Flute" 
    "/usr/local/share/Flute" 
    "${ophidian_source_dir}/dependencies/share/Flute"
)
find_file(PORT9 "PORT9.dat" PATHS 
    "/usr/share/Flute" 
    "/usr/local/share/Flute" 
    "${ophidian_source_dir}/dependencies/share/Flute"
)

message("Creating symlink FLUTE LUTs")
execute_process(
    COMMAND ln -f -s ${POWV9} ${PROJECT_BINARY_DIR}/test/.
    RESULT_VARIABLE result
)
execute_process(
    COMMAND ln -f -s ${PORT9} ${PROJECT_BINARY_DIR}/test/.
    RESULT_VARIABLE result
)

################################################################################
# Add test to ctest
################################################################################

enable_testing()
add_test(NAME unit_test COMMAND ophidian_tests)
