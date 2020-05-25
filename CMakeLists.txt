################################################################################
# This is the main CMakeLists file for the Ophidian library.
#
# Its main goals are:
#   - Set up global variables.
#   - Configure Ophidian dependencies (find_packege).
#   - Add subdirectories.
#
################################################################################
cmake_minimum_required(VERSION 3.10.2)

project(ophidian_meta VERSION 0.2.1)

set(ophidian_meta_VERSION ${PROJECT_VERSION})
set(ophidian_source_dir ${PROJECT_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/dependencies")

enable_testing()

add_subdirectory(ophidian)
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
