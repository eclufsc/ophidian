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

################################################################################
# Fetch files
################################################################################

# Fetch .cpp files for library creation
file(GLOB ophidian_floorplan_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_floorplan_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_floorplan ${ophidian_floorplan_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_floorplan PUBLIC
    ophidian_entity_system
    ophidian_parser
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_floorplan 
    DESTINATION lib
)

# Install rule for headers
install(
    FILES ${ophidian_floorplan_headers} 
    DESTINATION include/ophidian/floorplan
)
