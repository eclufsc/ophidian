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

################################################################################
# Set variables
################################################################################

# Set the include path for installed target
set(ophidian_placement_install_include_dir 
    ${ophidian_install_include_dir}/placement
)

################################################################################
# Fetch files
################################################################################

# Fetch .cpp files for library creation
file(GLOB ophidian_placement_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_placement_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_placement ${ophidian_placement_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_placement PUBLIC
    ophidian_parser
    ophidian_circuit
    ophidian_entity_system
    ophidian_standard_cell
    ophidian_geometry
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_placement PUBLIC
    $<BUILD_INTERFACE:      ${ophidian_source_dir}>
    $<INSTALL_INTERFACE:    include>
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_placement 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_placement_headers} 
    DESTINATION ${ophidian_placement_install_include_dir}
)
