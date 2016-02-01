

add_executable( placement_gui main.cpp ${HEADERS} )
target_link_libraries ( placement_gui LINK_PUBLIC entity standard_cell netlist placement )
 
 
 


