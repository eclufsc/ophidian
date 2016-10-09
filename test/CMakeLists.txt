set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE .)

add_subdirectory(entity_system)
add_subdirectory(circuit)
add_subdirectory(geometry)
add_subdirectory(parser)
add_subdirectory(interconnection)

add_executable( run_tests ${SOURCE} ${HEADERS} )
target_link_libraries(run_tests Catch ophidian_entitysystem ophidian_circuit ophidian_geometry ophidian_parser ophidian_interconnection emon)

include_directories(${CMAKE_SOURCE_DIR})
add_test(NAME unit_test COMMAND run_tests "~[Profiling]")


message("Creating symlink for test files")
execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/test/input_files ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
execute_process(COMMAND tar -zxvf superblue18.tar.gz
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/input_files
                RESULT_VARIABLE result )
