add_subdirectory(abacus)

find_package( Boost 1.59 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
add_library (legalization legalization.cpp legalization.h subrows.cpp subrows.h legalization_check.cpp legalization_check.h)
target_include_directories ( legalization PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries( legalization ${Boost_LIBRARIES} entity standard_cell netlist floorplan )