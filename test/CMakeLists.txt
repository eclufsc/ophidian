set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE .)

INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )
LINK_DIRECTORIES( ${THIRD_PARTY_PATH}/DEF/lib/ )

add_subdirectory(entity_system)
add_subdirectory(circuit)
add_subdirectory(geometry)
add_subdirectory(parser)

add_executable( run_tests ${SOURCE} ${HEADERS} )
target_link_libraries(run_tests Catch ophidian_entitysystem ophidian_circuit verilogparser ophidian_geometry ophidian_parser)

include_directories(${CMAKE_SOURCE_DIR})
add_test(NAME unit_test COMMAND run_tests "~[Profiling]")

add_custom_command(
        TARGET run_tests POST_BUILD
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/input_files ${CMAKE_BINARY_DIR}/test/input_files
        )
