cmake_minimum_required(VERSION 2.8.11)
# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})


project(placement_viewer)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

# Find the QtWidgets library
find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${SFML_INCLUDE_DIR})

# Tell CMake to create the placement_viewer executable
add_executable(placement_viewer WIN32 main.cpp mainwindow.cpp open_circuit_dialog.cpp application.cpp mysfmlcanvas.cpp qsfmlcanvas.cpp circuit.cpp fps.cpp resources.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(placement_viewer Qt5::Widgets entity placement netlist standard_cell timing interconnection timing-driven_placement ${SFML_LIBRARIES})
