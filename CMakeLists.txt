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
set(ophidian_VERSION 0.1)

# Set c++14 globally
set(CMAKE_CXX_STANDARD 14)

# Set Ophidian source dir
set(ophidian_source_dir ${PROJECT_SOURCE_DIR})

# Build ophidian_tests binary fully static
OPTION(OPHIDIAN_TESTS_FULLY_STATIC OFF)

# Run Uncrustify on project source
OPTION(UNCRUSTIFY_IT OFF)

# Enable uncrustify to check for code style
OPTION(RUN_UNCRUSTIFY_CHECK OFF)

# Set the uncrustify config file
set(ophidian_uncrustify_config ${ophidian_source_dir}/uncrustify.cfg)

################################################################################
# Set up installation variables
################################################################################

# Set ophidian shared libraries install path
set(ophidian_install_lib_dir lib)

# Set ophidian binary install path
set(ophidian_install_bin_dir bin)

# Set ophidian binary install path
set(ophidian_install_include_dir include)

# Set ophidian shared files path
set(ophidian_install_share_dir share/ophidian)

# Set ophidian copyright path
set(ophidian_install_copyright_dir shar/doc/ophidian)

################################################################################
# Set Linker RPATH options
################################################################################

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

################################################################################
# Install License
################################################################################

# License installation rule
install(
    FILES ${ophidian_source_dir}/LICENSE
    DESTINATION ${ophidian_install_share_dir}/copyright
)

################################################################################
# Handle raquirements
################################################################################

# Set Cmake module path for find_package
set(CMAKE_MODULE_PATH ${ophidian_source_dir}/cmake)

# Set Cmake prefix path for buildind with local dependencies
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${ophidian_source_dir}/dependencies)

# Find Boost
find_package(Boost 1.59 REQUIRED)

# Find Lemon
find_package(Lemon REQUIRED)

# Find Units
find_package(units REQUIRED CONFIG)

# Find DEF
find_package(DEF REQUIRED)

# Find LEF
find_package(LEF REQUIRED)

# Find Flute
find_package(Flute REQUIRED)

# Find Verilog-parser
find_package(verilog-parser REQUIRED)

################################################################################
# Project logic 
################################################################################

# Add subdirectories
add_subdirectory(3rdparty)
add_subdirectory(ophidian)
add_subdirectory(test)
