file(GLOB_RECURSE ophidian_gui_SRC
    "*.cpp"
    "*.ui"
)

INCLUDE_DIRECTORIES( ${THIRD_PARTY_SOURCE_DIR} )
LINK_DIRECTORIES( ${THIRD_PARTY_SOURCE_DIR}/DEF/lib/ ${THIRD_PARTY_SOURCE_DIR}/LEF/lib/ )

# Detect and add SFML
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
#find_package(SFML 2 REQUIRED network audio graphics window system)
#include_directories(${SFML_INCLUDE_DIR} ${THIRD_PARTY_SOURCE_DIR} CMAKE_MODULE_PATH)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets REQUIRED)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

#${THIRD_PARTY_SOURCE_DIR} ${OPHIDIAN_INCLUDE_DIRECTORIES}

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
#set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable( ophidian_gui ${ophidian_gui_SRC} )

target_link_libraries(ophidian_gui PUBLIC
    ${Qt5Widgets_LIBRARIES}
    ophidian_circuit
    ophidian_design
    ophidian_entity_system
    ophidian_floorplan
    ophidian_geometry
    ophidian_interconnection
    ophidian_parser
    ophidian_placement
    ophidian_standard_cell
    ophidian_timing
)

#${SFML_LIBRARIES}
#${SFML_DEPENDENCIES}
