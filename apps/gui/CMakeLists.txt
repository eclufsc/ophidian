################################################################################
# This is the CMakeLists file for the:
#
#   Ophidian GUI
#
# Its main goals are:
#   - Create an graphic user interface.
#
################################################################################
cmake_minimum_required(VERSION 3.10.2)

project(ophidian_gui VERSION ${ophidian_VERSION})

find_package(SFML 2 REQUIRED graphics window system)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(target_name ${PROJECT_NAME})

file(GLOB_RECURSE ophidian_gui_source
    "*.cpp"
)

file(GLOB_RECURSE ophidian_gui_userinterface
    "*.ui"
)

file(GLOB_RECURSE ophidian_gui_headers
    "*.h"
)

################################################################################
# Executable target
################################################################################
add_executable(${target_name})

target_sources(${target_name}
  PRIVATE
    ${ophidian_gui_source}
    ${ophidian_gui_userinterface}
)

target_link_libraries(${target_name}
  PRIVATE
    Qt5::Widgets
    SFML::GRAPHICS
    SFML::WINDOW
    SFML::SYSTEM
    ophidian::ophidian_circuit
    ophidian::ophidian_design
    ophidian::ophidian_entity_system
    ophidian::ophidian_floorplan
    ophidian::ophidian_geometry
    ophidian::ophidian_interconnection
    ophidian::ophidian_parser
    ophidian::ophidian_placement
)

################################################################################
# Installation rules
################################################################################
include(GNUInstallDirs)

install(TARGETS ophidian_gui
  RUNTIME
    COMPONENT Runtime
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

