cmake_minimum_required (VERSION 2.8.12.2)
project(placement_viewer)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)


# Include SFML headers
include_directories(${SFML_INCLUDE_DIR})

# Tell CMake to create the helloworld executable
add_executable(placement_viewer main.cpp mainwindow.cpp open_circuit_dialog.cpp qsfmlcanvas.cpp mysfmlcanvas.cpp application.cpp )

# Use the Widgets module from Qt 5 and SFML Libraries
target_link_libraries(placement_viewer Qt5::Widgets ${SFML_LIBRARIES} entity netlist standard_cell placement)
