cmake_minimum_required(VERSION 3.10.2)

project(ophidian_interconnection_tests VERSION ${ophidian_VERSION})

find_package(Catch2 REQUIRED)

set(target_name ${PROJECT_NAME})

set(install_component_prefix ${PROJECT_NAME}_)
set(install_component_bin ${install_component_prefix}RunTime)

file(GLOB source_files
  "src/*.cpp"
)

################################################################################
# Executable target
################################################################################
add_executable(${target_name} ${source_files})

target_link_libraries(${target_name}
  PRIVATE Catch
  PRIVATE ophidian_interconnection
)

execute_process(
    COMMAND ln -f -s ${ophidian_source_dir}/input_files ${PROJECT_BINARY_DIR}
    RESULT_VARIABLE result
)

enable_testing()
add_test(NAME ${target_name}
  COMMAND ${target_name}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

################################################################################
# Installation rules
################################################################################
include(GNUInstallDirs)

install(TARGETS ${target_name}
  RUNTIME
    COMPONENT ${install_component_bin}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
