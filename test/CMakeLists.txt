set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE .)

INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )
LINK_DIRECTORIES( ${THIRD_PARTY_PATH}/DEF/lib/ ${THIRD_PARTY_PATH}/LEF/lib/ )

add_subdirectory(entity_system)
add_subdirectory(circuit)
add_subdirectory(geometry)
add_subdirectory(parser)
add_subdirectory(util)
add_subdirectory(interconnection)
add_subdirectory(timing)

add_executable( run_tests ${SOURCE} ${HEADERS} )
target_link_libraries(run_tests Catch ophidian_entitysystem ophidian_circuit ophidian_geometry ophidian_parser ophidian_interconnection ophidian_timing emon)

include_directories(${CMAKE_SOURCE_DIR})
add_test(NAME unit_test COMMAND run_tests "~[Profiling] ~[superblue18obs]")

add_custom_command(
        TARGET run_tests POST_BUILD
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/input_files ${CMAKE_CURRENT_BINARY_DIR}/input_files
        )

message("Creating symlink for test files")
execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/test/input_files ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
execute_process(COMMAND tar -zxvf superblue18.tar.gz
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/input_files
                RESULT_VARIABLE result )

message("Creating symlink FLUTE LUTs")
execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/3rdparty/Flute/PORT9.dat ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
execute_process(COMMAND ln -f -s ${CMAKE_SOURCE_DIR}/3rdparty/Flute/POWV9.dat ${CMAKE_BINARY_DIR}/test/.
                RESULT_VARIABLE result )
