################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::entity_system
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
file(GLOB ophidian_entity_system_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_entity_system_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_entity_system ${ophidian_entity_system_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_entity_system PUBLIC 
    lemon
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_entity_system 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_entity_system_headers} 
    DESTINATION ${ophidian_install_include_dir}/entity_system
)
