################################################################################
# This is the CMakeLists file for the Ophidian library directory.
#
#   Each namespace should be in its own directory with its own CMakeLists.txt 
# with rules for target generation and installation. Unless you do not want to 
# make it a dinamic lib.
################################################################################

# Add namespace directories for lib generation 
add_subdirectory(circuit)
add_subdirectory(design)
add_subdirectory(entity_system)
add_subdirectory(floorplan)
add_subdirectory(geometry)
add_subdirectory(interconnection)
add_subdirectory(parser)
add_subdirectory(placement)
add_subdirectory(standard_cell)
add_subdirectory(timing)
add_subdirectory(timingdriven_placement)
