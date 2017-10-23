################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::util 
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
set(ophidian_util_install_include_dir 
    ${ophidian_install_include_dir}/util
)

################################################################################
# Fetch files
################################################################################

# Fetch .h files for library creation
file(GLOB ophidian_timing_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_util INTERFACE ophidian_timing_headers)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_util PUBLIC
    $<BUILD_INTERFACE:      ${ophidian_source_dir}>
    $<INSTALL_INTERFACE:    include>
)

################################################################################
# Installation rules
################################################################################

# Install rule for headers
install(
    FILES ${ophidian_timing_headers} 
    DESTINATION ${ophidian_util_install_include_dir}
)
