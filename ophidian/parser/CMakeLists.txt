################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::parser
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

# Fetch .ccp files for library creation
file(GLOB ophidian_parser_source
    "*.cpp"
)

# Fetch .h files for library creation
file(GLOB ophidian_parser_headers
    "*.h"
)

################################################################################
# Library target
################################################################################

#link_directories(${third_party_source_dir}/DEF/lib)
#link_directories(${third_party_source_dir}/LEF/lib)

# Add library target
add_library(ophidian_parser ${ophidian_parser_source})

# Tell cmake target's dependencies
add_dependencies(ophidian_parser def_parser lef_parser)
target_link_libraries(ophidian_parser PUBLIC 
    verilogparser
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_parser 
    DESTINATION ${ophidian_install_lib_dir}
)

# Install rule for headers
install(
    FILES ${ophidian_parser_headers} 
    DESTINATION ${ophidian_install_include_dir}/parser
)
