set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/run_regression.cpp
        )

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

add_subdirectory(density)
add_subdirectory(abacus)


add_executable( run_regression ${SOURCE} ${HEADERS} )

target_link_libraries ( run_regression LINK_PUBLIC entity_system standard_cell netlist parsing placement floorplan interconnection timing timing-driven_placement density legalization abacus routing clock_tree_synthesis register_clustering emon )

add_custom_command(
        TARGET run_regression POST_BUILD
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/input_files ${CMAKE_CURRENT_BINARY_DIR}/input_files
        COMMAND ln -sf ${THIRD_PARTY_PATH} ${CMAKE_CURRENT_BINARY_DIR}
)
