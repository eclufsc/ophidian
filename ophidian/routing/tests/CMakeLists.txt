cmake_minimum_required(VERSION 3.10.2)

project(ophidian_routing_tests VERSION ${ophidian_VERSION})

find_package(Catch2 REQUIRED)

set(target_name ${PROJECT_NAME})

file(GLOB source_files
  "src/*.cpp"
)

################################################################################
# Executable target
################################################################################
add_executable(${target_name} ${source_files})

target_link_libraries(${target_name}
  PRIVATE Catch
  PUBLIC ophidian_routing
)

message("Creating symlink for test files")
execute_process(
    COMMAND ln -f -s ${ophidian_source_dir}/test/input_files ${PROJECT_BINARY_DIR}
    RESULT_VARIABLE result
)
