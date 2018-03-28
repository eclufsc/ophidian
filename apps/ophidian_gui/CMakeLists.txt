file(GLOB_RECURSE ophidian_gui_SRC
    "*.cpp"
    "*.ui"
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable( ophidian_gui ${ophidian_gui_SRC} )

target_link_libraries(ophidian_gui PUBLIC
    PRIVATE Qt5::Widgets
    PRIVATE SFML::sfml
    PRIVATE ophidian_circuit
    PRIVATE ophidian_design
    PRIVATE ophidian_entity_system
    PRIVATE ophidian_floorplan
    PRIVATE ophidian_geometry
    PRIVATE ophidian_interconnection
    PRIVATE ophidian_parser
    PRIVATE ophidian_placement
    PRIVATE ophidian_standard_cell
)

#${SFML_LIBRARIES}
#${SFML_DEPENDENCIES}
