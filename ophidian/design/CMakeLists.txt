################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::design
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
file(GLOB ophidian_design_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_design_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_design ${ophidian_design_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_design PUBLIC 
    ophidian_circuit 
    ophidian_floorplan 
    ophidian_standard_cell 
    ophidian_placement 
    ophidian_parser
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_design 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_design_headers} 
    DESTINATION ${ophidian_install_include_dir}/design
)
