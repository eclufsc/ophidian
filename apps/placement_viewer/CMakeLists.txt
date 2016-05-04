cmake_minimum_required(VERSION 2.8.11)

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
