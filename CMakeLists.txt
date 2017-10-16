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

# Set project_name global variable
set(project_name "ophidian")

# Set project name 
project(${project_name})

# Set c++14 globaly 
set(CMAKE_CXX_STANDARD 14)

# Set Ophidian source dir
set(ophidian_source_dir ${PROJECT_SOURCE_DIR})

# Set the dependencies directory
set(third_party_source_dir ${ophidian_source_dir}/3rdparty)

# Set include dirs for the Lemon library
set(lemon_include_dir 
    ${PROJECT_SOURCE_DIR}/3rdparty/Lemon/
    ${PROJECT_BINARY_DIR}/3rdparty/Lemon/
    ${PROJECT_BINARY_DIR}/3rdparty/Lemon/lemon
)

# Set the global include directories variable
set(ophidian_include_dir
    ${ophidian_source_dir}
    ${third_party_source_dir}
    ${lemon_include_dir}
)

################################################################################
# Set up installation variables
################################################################################

# Set Cmake install prefix 
set(CMAKE_INSTALL_PREFIX /usr/local)

# Set ophidian shared libraries install path
set(ophidian_install_lib_dir lib/${project_name})

# Set ophidian binary install path
set(ophidian_install_bin_dir bin)

# Set ophidian include install path
set(ophidian_install_include_dir include/${project_name})

################################################################################
# Handle raquirements
################################################################################

# Find Boost
find_package(Boost 1.59 REQUIRED)

# Find OpenMP if aplicable
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

################################################################################
# Project logic 
################################################################################

# Set global compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")

# Set option build shared libs
option(BUILD_SHARED_LIBS "on/off -> (build every non statc lib shared)/(build everything static)" ON)

# Set the global include directories
include_directories(${ophidian_include_dir})

# First add 3rdparty software to build
add_subdirectory(3rdparty)

# Then add Ophidian
add_subdirectory(ophidian)

# Then add test
add_subdirectory(test)
