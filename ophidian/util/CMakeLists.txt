################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::util
#
# Its main goals are:
#   - Fetch library files.
#   - Add target.
#       `- Set target_include_path.
#       `- Set target_link_libraries.
#       `- Set target_compiler_options.
#   - Define installation parameters.
#       `- Install targets.
#       `- Install headers.
#
################################################################################

################################################################################
# Set variables
################################################################################

# Set the include path for installed target
set(ophidian_util_install_include_dir 
    ${ophidian_install_include_dir}/ophidian/util
)

################################################################################
# Fetch files
################################################################################

# Fetch .h files for library creation
file(GLOB ophidian_util_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_util INTERFACE)

# Tell cmake target's dependencies
target_link_libraries(ophidian_util
    INTERFACE units
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_util INTERFACE
    $<BUILD_INTERFACE:      ${ophidian_source_dir}>
    $<INSTALL_INTERFACE:    include>
)

################################################################################
# Installation rules
################################################################################

# Install rule for headers
install(
    FILES ${ophidian_util_headers} 
    DESTINATION ${ophidian_util_install_include_dir}
)
