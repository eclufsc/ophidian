################################################################################
# This is the main CMakeLists file for the Ophidian library.
#
# Its main goals are:
#   - Set up global variables.
#   - Configure Ophidian dependencies (find_packege).
#   - Add subdirectories.
#
################################################################################

################################################################################
# Set up global variables
################################################################################

# Set up minimal cmake version
cmake_minimum_required(VERSION 3.10.2)

# Set project version
set(ophidian_VERSION 0.2.1)

# Set project name
project(ophidian VERSION ${ophidian_VERSION})

# Set c++17 globally
set(CMAKE_CXX_STANDARD 17)

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

# Build graphic user interface of ophidian
OPTION(OPHIDIAN_GUI OFF)

# Enable applications' build
IF(OPHIDIAN_GUI)
    set(OPHIDIAN_APPS ON)
ELSE(OPHIDIAN_GUI)
    set(OPHIDIAN_APPS OFF)
ENDIF(OPHIDIAN_GUI)

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
set(ophidian_install_copyright_dir share/doc/ophidian)

################################################################################
# Handle raquirements
################################################################################

# Set Cmake module path for find_package
list(APPEND CMAKE_MODULE_PATH "${ophidian_source_dir}/cmake")

# Set Cmake prefix path for building with local dependencies
list(APPEND CMAKE_PREFIX_PATH "${ophidian_source_dir}/dependencies")

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
find_package(flute REQUIRED)

# Find Verilog-parser
find_package(verilogparser REQUIRED)

# Find GUI Dependencies
IF(OPHIDIAN_GUI)
    # Find SFML
    find_package(SFML 2 REQUIRED graphics window system)

    # Find Qt5
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
ENDIF(OPHIDIAN_GUI)

################################################################################
# Project logic
################################################################################

# Add subdirectories
add_subdirectory(3rdparty)
add_subdirectory(ophidian)
add_subdirectory(test)
add_subdirectory(experiments)

IF(OPHIDIAN_APPS)
    add_subdirectory(apps)
ENDIF(OPHIDIAN_APPS)
