cmake_minimum_required(VERSION 3.17)

project(iccad19gr)

find_package(Catch2 REQUIRED)

set(target_name ${PROJECT_NAME})

file(GLOB source_files
  "src/*.cpp"
)

################################################################################
# Executable target
################################################################################
add_executable(${target_name})

target_sources(${target_name}
  PRIVATE
    ${source_files}
)

target_link_libraries(${target_name}
  PRIVATE
    Catch
    ophidian::ophidian_circuit
    ophidian::ophidian_design
    ophidian::ophidian_entity_system
    ophidian::ophidian_floorplan
    ophidian::ophidian_geometry
    ophidian::ophidian_interconnection
    ophidian::ophidian_routing
    ophidian::ophidian_placement
    ophidian::ophidian_parser
)

message("Creating symlink for test files")
execute_process(
    COMMAND ln -f -s ${ophidian_source_dir}/input_files ${PROJECT_BINARY_DIR}
    RESULT_VARIABLE result
)
