cmake_minimum_required(VERSION 3.10.2)

project(ophidian_parser_tests VERSION ${ophidian_VERSION})

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
  PRIVATE ophidian_parser
)
