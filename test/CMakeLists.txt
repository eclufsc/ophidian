set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp
)
add_subdirectory (data_structures)
add_subdirectory (standard_cell)
add_subdirectory (netlist)
add_subdirectory (placement)
add_subdirectory(floorplan)

add_executable( run_tests ${SOURCE} ${HEADERS} )

target_link_libraries ( run_tests LINK_PUBLIC entity standard_cell netlist placement floorplan )

