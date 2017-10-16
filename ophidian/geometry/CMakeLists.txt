################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::geometry
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
file(GLOB ophidian_geometry_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_geometry_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_geometry ${ophidian_geometry_source})

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_geometry 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_geometry_headers} 
    DESTINATION ${ophidian_install_include_dir}/geometry
)
