cmake_minimum_required(VERSION 2.8.11)

project(uddac2016)


# Detect and add SFML
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR} ${THIRD_PARTY_PATH} ${THIRD_PARTY_PATH}/QtPropertyBrowser/src/)


LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

# Find the QtWidgets library
find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(SOURCES
    main.cpp
    controller.cpp
    canvas.cpp
    random_purple_cell_painter.cpp
    application.cpp
    mainwindow.cpp
    openlefdefdialog.cpp
    mainwindowmenubar.cpp
)


# Tell CMake to create the uddac2016 executable
add_executable(uddac2016 WIN32 ${SOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(uddac2016 Qt5::Widgets timing-driven_placement timing abacus interconnection floorplan standard_cell placement parsing gui entity_system ${SFML_LIBRARIES} LEF DEF  )
