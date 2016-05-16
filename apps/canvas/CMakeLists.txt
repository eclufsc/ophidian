cmake_minimum_required(VERSION 2.8.11)
# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)

include_directories(${SFML_INCLUDE_DIR})


project(canvas)

# Find the QtWidgets library
find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(SOURCES
    main.cpp
    mainwindow.cpp
)


# Tell CMake to create the canvas executable
add_executable(canvas WIN32 ${SOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(canvas Qt5::Widgets gui entity ${SFML_LIBRARIES}  )
