################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::placement
#
# Its main goals are:
#   - Fetch library files.
#   - Add compilation target.
#   - Add target specific include paths.
#   - Add compilation target dependencies. 
#       `-(this is a must due to telling cmake what should it compile fisrt).
#   - Define installation parameters.
################################################################################

# Fetch .ccp files for library creation
file(GLOB ophidian_placement_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_placement ${ophidian_placement_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_placement PUBLIC
    ophidian_parser
    ophidian_circuit
    ophidian_entity_system
    ophidian_standard_cell
    ophidian_geometry
)

# Instal parameters for make install
install(TARGETS ophidian_placement DESTINATION lib)
install(FILES Placement.h Library.h DESTINATION include/ophidian/placement)
