find_package( Boost 1.59 )

cmake_policy(SET CMP0015 NEW)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/si2/lib/)  

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/si2/include )
add_library (timing elmore.cpp liberty.cpp library.cpp library_timing_arcs.cpp graph_arcs_timing.cpp graph_nodes_timing.cpp sta_timing_point_calculator.cpp sta_timing_arc_calculator.cpp static_timing_analysis.cpp graph.cpp graph_builder.cpp sta_arc_calculator.cpp elmore_second_moment.cpp design_constraints.cpp simple_design_constraint.cpp )
target_include_directories ( timing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

link_directories( 3rdparty/si2/lib/ )
target_link_libraries( timing ${Boost_LIBRARIES} emon ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/si2/lib/libsi2dr_liberty.a )
