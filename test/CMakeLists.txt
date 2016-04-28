set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp
)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)


add_subdirectory (standard_cell)
add_subdirectory (netlist)
add_subdirectory (placement)
add_subdirectory (geometry)
add_subdirectory (regression)
add_subdirectory (floorplan)
add_subdirectory (interconnection)
add_subdirectory (timing)
add_subdirectory (timing-driven_placement)
add_subdirectory(density)

add_executable( run_tests ${SOURCE} ${HEADERS} )

target_link_libraries ( run_tests LINK_PUBLIC entity standard_cell netlist placement floorplan interconnection timing timing-driven_placement density legalization emon )

