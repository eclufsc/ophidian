################################################################################
# This is the main CMakeLists file for the Ophidian library. 
#
# Its main goals are:
#   - Set up global variables.
#   - Configure Ophidian dependencies (find_packege).
#   - Handle Ophidian compilation logic. 
#   - Add subdirectories.
#
################################################################################

################################################################################
# Set up global variables
################################################################################

# Set up minimal cmake version
cmake_minimum_required(VERSION 3.5.2)

# Set project name 
project(ophidian)

# Set project version
set(ophidian_VERSION 0.1.1)

# Set c++14 globally 
set(CMAKE_CXX_STANDARD 14)

# Set Ophidian source dir
set(ophidian_source_dir ${PROJECT_SOURCE_DIR})

################################################################################
# Set up installation variables
################################################################################

# Set Cmake install prefix 
set(CMAKE_INSTALL_PREFIX /usr/local)

# Set ophidian shared libraries install path
set(ophidian_install_lib_dir lib)

# Set ophidian binary install path
set(ophidian_install_bin_dir bin)

# Set ophidian include install path
set(ophidian_install_include_dir include)

################################################################################
# Handle raquirements
################################################################################

# Set Cmake module path for find_package
set(CMAKE_MODULE_PATH ${ophidian_source_dir}/cmake)

# Find Boost
find_package(Boost 1.59 REQUIRED)

# Find Lemon
find_package(Lemon REQUIRED)

# Find Units
find_package(units 2.3.1 REQUIRED CONFIG)

################################################################################
# Project logic 
################################################################################

# Add subdirectories
add_subdirectory(3rdparty)
add_subdirectory(ophidian)
add_subdirectory(test)
