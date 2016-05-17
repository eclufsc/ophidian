set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp
        )

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)


add_subdirectory (standard_cell)
add_subdirectory (netlist)
add_subdirectory (placement)
add_subdirectory (geometry)
#add_subdirectory (regression)
add_subdirectory (floorplan)
add_subdirectory (interconnection)
add_subdirectory (timing)
add_subdirectory (timing-driven_placement)
add_subdirectory (density)
add_subdirectory (routing)

add_executable( run_tests ${SOURCE} ${HEADERS} )

target_link_libraries ( run_tests LINK_PUBLIC entity_system standard_cell netlist parsing placement floorplan interconnection timing timing-driven_placement density legalization abacus routing emon )
add_subdirectory (parsing)

add_custom_command(
        TARGET run_tests POST_BUILD
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/input_files ${CMAKE_CURRENT_BINARY_DIR}/input_files
        COMMAND ln -sf ${THIRD_PARTY_PATH} ${CMAKE_CURRENT_BINARY_DIR}

)
