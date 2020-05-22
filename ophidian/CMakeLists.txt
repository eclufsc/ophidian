cmake_minimum_required(VERSION 3.13)

project(ophidian VERSION ${ophidian_meta_VERSION})

set(ophidian_VERSION ${PROJECT_VERSION})

add_subdirectory(circuit)
add_subdirectory(design)
add_subdirectory(entity_system)
add_subdirectory(floorplan)
add_subdirectory(geometry)
add_subdirectory(interconnection)
add_subdirectory(parser)
add_subdirectory(placement)
add_subdirectory(routing)
add_subdirectory(util)

set(target_name ${PROJECT_NAME})

set(export_name ${PROJECT_NAME}-export)
set(export_namespace ophidian::)
set(package_config_name ${PROJECT_NAME}-config)
set(package_config_version_name ${package_config_name}-version)

################################################################################
# Library target
################################################################################
add_library(${target_name} INTERFACE)
add_library(${export_namespace}${target_name} ALIAS ${target_name})

target_link_libraries(${target_name}
  INTERFACE
    ophidian::ophidian_circuit
    ophidian::ophidian_parser
    ophidian::ophidian_interconnection
    ophidian::ophidian_entity_system
    ophidian::ophidian_geometry
)

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
