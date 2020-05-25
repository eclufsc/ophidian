cmake_minimum_required(VERSION 3.13)

project(ophidian_entity_system VERSION ${ophidian_meta_VERSION})

find_package(Lemon REQUIRED)

set(target_name ${PROJECT_NAME})

set(export_name ${PROJECT_NAME}-export)
set(export_namespace ophidian::)

set(package_config_name ${PROJECT_NAME}-config)
set(package_config_version_name ${package_config_name}-version)

set(install_component_prefix ${PROJECT_NAME}_)
set(install_component_dev ${install_component_prefix}Development)
set(install_component_lib ${install_component_prefix}Libraries)
set(install_component_bin ${install_component_prefix}RunTime)

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
)

target_compile_features(${target_name}
  PUBLIC
    cxx_std_17
)

set_target_properties(${target_name}
  PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

enable_testing()
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
  RUNTIME
    COMPONENT ${install_component_bin}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY
    COMPONENT ${install_component_lib}
    NAMELINK_COMPONENT ${install_component_dev}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE
    COMPONENT ${install_component_lib}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  COMPONENT ${install_component_dev}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  FILES
    ${PROJECT_BINARY_DIR}/${package_config_name}.cmake
    ${PROJECT_BINARY_DIR}/${package_config_version_name}.cmake
  COMPONENT ${install_component_dev}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake
)

install(EXPORT ${export_name}
  COMPONENT ${install_component_dev}
  NAMESPACE ${export_namespace}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake
)
