set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.cpp
)
add_subdirectory (netlist)
add_subdirectory (data_structures)
add_executable( run_tests ${SOURCE} ${HEADERS} )
target_link_libraries ( run_tests LINK_PUBLIC netlist )

