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

# Fetch tests (except main.cpp) files recursevely
file(GLOB_RECURSE ophidian_experiments_test
    "*.h"
    "*.cpp"
)

list(REMOVE_ITEM ophidian_experiments_test "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")



# only main
file(GLOB_RECURSE ophidian_experiments_source
    "main.cpp"
    "date21.h"
    "date21.cpp"
)

# Add main target
add_executable(cell_move_router ${ophidian_experiments_source})

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
    PRIVATE ${CPLEX_LIBRARIES}
    PRIVATE dl
)

# Link target dependencies
target_link_libraries(cell_move_router ${LIBS})

# Link target dependencies
target_link_libraries(run_tests ${LIBS})


if(OPHIDIAN_EXPERIMENTS_FULLY_STATIC)
    # Link EVERYTHING staticly!
    set_target_properties(cell_move_router PROPERTIES
        LINK_FLAGS "-static -static-libgcc -static-libstdc++ -pthread"
    )
endif()

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

