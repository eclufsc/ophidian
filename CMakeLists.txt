################################################################################
# This is the main CMakeLists file for the Ophidian library.
#
# Its main goals are:
#   - Set up global variables.
#   - Configure Ophidian dependencies (find_packege).
#   - Add subdirectories.
#
################################################################################
cmake_minimum_required(VERSION 3.13)

project(ophidian_meta VERSION 0.2.1)

set(ophidian_meta_VERSION ${PROJECT_VERSION})
set(ophidian_source_dir ${PROJECT_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/dependencies")

enable_testing()

add_subdirectory(ophidian)
add_subdirectory(ophidian_circuit)
add_subdirectory(ophidian_design)
add_subdirectory(ophidian_entity_system)
add_subdirectory(ophidian_floorplan)
add_subdirectory(ophidian_geometry)
add_subdirectory(ophidian_interconnection)
add_subdirectory(ophidian_parser)
add_subdirectory(ophidian_placement)
add_subdirectory(ophidian_routing)
add_subdirectory(ophidian_util)
add_subdirectory(experiments)

OPTION(OPHIDIAN_GUI OFF)
IF(OPHIDIAN_GUI)
  set(OPHIDIAN_APPS ON)
ELSE(OPHIDIAN_GUI)
  set(OPHIDIAN_APPS OFF)
ENDIF(OPHIDIAN_GUI)

IF(OPHIDIAN_APPS)
    add_subdirectory(apps)
ENDIF(OPHIDIAN_APPS)
