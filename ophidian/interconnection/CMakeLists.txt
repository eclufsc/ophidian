################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::interconnection
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
file(GLOB ophidian_interconnection_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_interconnection_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_interconnection ${ophidian_interconnection_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_interconnection PUBLIC 
    flute 
    pthread 
    lemon
    ophidian_geometry
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_interconnection 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_interconnection_headers} 
    DESTINATION ${ophidian_install_include_dir}/interconnection
)
