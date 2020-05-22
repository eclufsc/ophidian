################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::interconnection
#
# Its main goals are:
#   - Fetch library files.
#   - Add target.
#       `- Set target_include_path.
#       `- Set target_link_libraries.
#       `- Set target_compiler_options.
#   - Define installation parameters.
#       `- Install targets.
#       `- Install headers.
#
################################################################################
cmake_minimum_required(VERSION 3.10.2)

project(ophidian_interconnection VERSION ${ophidian_VERSION})

set(target_name ${PROJECT_NAME})

set(export_name ${PROJECT_NAME}-export)
set(export_namespace ophidian::)
set(package_config_name ${PROJECT_NAME}-config)
set(package_config_version_name ${package_config_name}-version)

file(GLOB source_files
  "src/*.cpp"
)

################################################################################
# Library target
################################################################################
add_library(${target_name} ${source_files})
add_library(${export_namespace}${target_name} ALIAS ${target_name})

target_sources(${target_name}
  PRIVATE
    ${source_files}
)

target_include_directories(${target_name}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${target_name}
  PUBLIC
    Lemon::lemon
    ophidian::ophidian_geometry
  PRIVATE
    Flute::flute
)

target_compile_features(${target_name}
  PUBLIC
    cxx_std_17
)

set_target_properties(${target_name}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
)

add_subdirectory(tests)

################################################################################
# Installation rules
################################################################################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

export(TARGETS ${target_name}
  NAMESPACE ${export_namespace}
  FILE ${PROJECT_BINARY_DIR}/${export_name}.cmake
)

write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${package_config_version_name}.cmake
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/package-config.cmake.in
  ${PROJECT_BINARY_DIR}/${package_config_name}.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIRA}/${PROJECT_NAME}/cmake
)

install(TARGETS ${target_name}
  EXPORT ${export_name}
  LIBRARY
    COMPONENT Libraries
    NAMELINK_COMPONENT Development
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE
    COMPONENT Libraries
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  COMPONENT Development
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  FILES
    ${PROJECT_BINARY_DIR}/${package_config_name}.cmake
    ${PROJECT_BINARY_DIR}/${package_config_version_name}.cmake
  COMPONENT Development
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake
)

install(EXPORT ${export_name}
  COMPONENT Development
  NAMESPACE ${export_namespace}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake
)
