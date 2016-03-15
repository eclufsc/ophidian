set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp
)



add_subdirectory (data_structures)
add_subdirectory (standard_cell)
add_subdirectory (netlist)
add_subdirectory (placement)
add_subdirectory (geometry)
add_subdirectory (regression)
add_subdirectory (floorplan)
add_subdirectory (interconnection)
add_subdirectory (timing)
add_subdirectory (timing-driven_placement)

add_executable( run_tests ${SOURCE} ${HEADERS} )

target_link_libraries ( run_tests LINK_PUBLIC entity standard_cell netlist placement floorplan timing interconnection timing-driven_placement emon )

