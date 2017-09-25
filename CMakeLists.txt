################################################################################
# This is the main CMakeLists file for the Ophidian library.
#
# Its main goals are:
#   - set up global variables.
#   - configure Ophidian compilation requirements (find_packege)
#   - handle Ophidian compilation logic
#   - set up the global include directories
#   - add subdirectories
################################################################################

################################################################################
# Set up global variables
################################################################################

# Set up minimal cmake version
cmake_minimum_required(VERSION 3.2.2)

# Set PROJECT_NAME global variable
set(PROJECT_NAME "ophidian")

# Set projec name
project(${PROJECT_NAME})

# Set c++14 globaly
set(CMAKE_CXX_STANDARD 14)

# Set Ophidian source dir
set(OPHIDIAN_SOURCE_DIR ${PROJECT_SOURCE_DIR})

# Set the dependencies directory
set(THIRD_PARTY_SOURCE_DIR ${OPHIDIAN_SOURCE_DIR}/3rdparty)

# Set include dirs for the Lemon library
set(LEMON_INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}/3rdparty/Lemon/
    ${PROJECT_BINARY_DIR}/3rdparty/Lemon/
    ${PROJECT_BINARY_DIR}/3rdparty/Lemon/lemon
)

# Set the global include directories variable
set(OPHIDIAN_INCLUDE_DIRECTORIES
    ${OPHIDIAN_SOURCE_DIR}
    ${THIRD_PARTY_SOURCE_DIR}
    ${LEMON_INCLUDE_DIR}
)

################################################################################
# Handle raquirements
################################################################################

# Find Boost
find_package(Boost 1.59 REQUIRED)

# Find OpenMp if aplicable
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

################################################################################
# Project logic
################################################################################

# Set globla compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")

# Set option build shared libs
option(BUILD_SHARED_LIBS "on/off -> (build every non statc lib shared)/(build everything static)" ON)

# Set the global include directories
include_directories(${OPHIDIAN_INCLUDE_DIRECTORIES})

# First add 3rdparty software to build
add_subdirectory(3rdparty)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/verilog-parser/src/)

# Then add Ophidian
add_subdirectory(ophidian)

# Then add test
add_subdirectory(test)

# Then add app
add_subdirectory(apps)
