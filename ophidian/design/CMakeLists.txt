################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::design
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
set(ophidian_design_install_include_dir 
    ${ophidian_install_include_dir}/ophidian/design
)

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
# Uncrustify
################################################################################

set(uncrustify_files ${ophidian_design_source} ${ophidian_design_headers})

if(UNCRUSTIFY_IT)
    include(uncrustify_helper)
    uncrustify_it(${ophidian_uncrustify_config} "${uncrustify_files}")
endif()

if(RUN_UNCRUSTIFY_CHECK)
    include(uncrustify_helper)
    uncrustify_check(${ophidian_uncrustify_config} "${uncrustify_files}")
endif()

################################################################################
# Library target
################################################################################

# Add library target
add_library(ophidian_design SHARED ${ophidian_design_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_design
    PUBLIC ophidian_circuit 
    PUBLIC ophidian_floorplan 
    PUBLIC ophidian_standard_cell 
    PUBLIC ophidian_placement 
    PUBLIC ophidian_parser
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_design PUBLIC
    $<BUILD_INTERFACE:${ophidian_source_dir}>
    $<INSTALL_INTERFACE:include>
)

# Add library target
add_library(ophidian_design_static STATIC ${ophidian_design_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_design_static
    PUBLIC ophidian_circuit_static 
    PUBLIC ophidian_floorplan_static
    PUBLIC ophidian_standard_cell_static
    PUBLIC ophidian_placement_static
    PUBLIC ophidian_parser_static
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_design_static PUBLIC
    $<BUILD_INTERFACE:${ophidian_source_dir}>
    $<INSTALL_INTERFACE:include>
)

################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_design ophidian_design_static 
    DESTINATION ${ophidian_install_lib_dir}
    EXPORT ophidian-targets
)

# Install rule for headers
install(
    FILES ${ophidian_design_headers} 
    DESTINATION ${ophidian_design_install_include_dir}
)
