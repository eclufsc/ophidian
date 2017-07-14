file(GLOB_RECURSE ophidian_run_tests_SRC  
    "*.cpp"
)

add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE .)

add_executable(run_tests ${ophidian_run_tests_SRC})
target_link_libraries(run_tests PUBLIC 
    Catch 
    ophidian_circuit 
    ophidian_design 
    ophidian_entitysystem 
    ophidian_floorplan 
    ophidian_geometry 
    ophidian_interconnection 
    ophidian_parser 
    ophidian_placement 
    ophidian_standard_cell 
    ophidian_timing 
)

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

add_test(NAME unit_test COMMAND run_tests "~[Profiling] ~[superblue18obs]")
