################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::interconnection
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
set(ophidian_interconnection_install_include_dir 
    ${ophidian_install_include_dir}/ophidian/interconnection
)

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
target_link_libraries(ophidian_interconnection
    PUBLIC ophidian_geometry
    PUBLIC flute 
    PUBLIC pthread 
    PUBLIC lemon
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_interconnection PUBLIC
    $<BUILD_INTERFACE:      ${ophidian_source_dir}>
    $<INSTALL_INTERFACE:    include>
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_interconnection 
    DESTINATION ${ophidian_install_lib_dir}
    EXPORT ophidian
)

# Install rule for headers
install(
    FILES ${ophidian_interconnection_headers} 
    DESTINATION ${ophidian_interconnection_install_include_dir}
)
