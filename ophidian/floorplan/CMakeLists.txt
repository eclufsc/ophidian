################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::floorplan
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
file(GLOB ophidian_floorplan_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_floorplan ${ophidian_floorplan_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_floorplan PUBLIC
    ophidian_entity_system
    ophidian_parser
)

# Instal parameters for make install
install(TARGETS ophidian_floorplan DESTINATION lib)
install(FILES Floorplan.h DESTINATION include/ophidian/floorplan)
