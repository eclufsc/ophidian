################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::placement
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
set(ophidian_placement_install_include_dir 
    ${ophidian_install_include_dir}/ophidian/placement
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
# Uncrustify
################################################################################

set(uncrustify_files ${ophidian_placement_source} ${ophidian_placement_headers})

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
add_library(ophidian_placement SHARED ${ophidian_placement_source})

# Set shared library version, this will make cmake create a link
set_target_properties(ophidian_placement PROPERTIES
    VERSION ${ophidian_VERSION}
    SOVERSION ${ophidian_VERSION}
)

# Tell cmake target's dependencies
target_link_libraries(ophidian_placement
    PUBLIC ophidian_parser
    PUBLIC ophidian_circuit
    PUBLIC ophidian_entity_system
    PUBLIC ophidian_standard_cell
    PUBLIC ophidian_geometry
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_placement PUBLIC
    $<BUILD_INTERFACE:${ophidian_source_dir}>
    $<INSTALL_INTERFACE:include>
)

# Add library target
add_library(ophidian_placement_static STATIC ${ophidian_placement_source})

# Tell cmake target's dependencies
target_link_libraries(ophidian_placement_static
    PUBLIC ophidian_parser_static
    PUBLIC ophidian_circuit_static
    PUBLIC ophidian_entity_system_static
    PUBLIC ophidian_standard_cell_static
    PUBLIC ophidian_geometry_static
)

# Tell cmake the path to look for include files for this target
# This is transitive, so any targets linked with this one will
# search for headers in the respective paths
target_include_directories(ophidian_placement_static PUBLIC
    $<BUILD_INTERFACE:${ophidian_source_dir}>
    $<INSTALL_INTERFACE:include>
)


################################################################################
# Installation rules
################################################################################

# Install rule for target
install(
    TARGETS ophidian_placement ophidian_placement_static 
    DESTINATION ${ophidian_install_lib_dir}
    EXPORT ophidian-targets
)

# Install rule for headers
install(
    FILES ${ophidian_placement_headers} 
    DESTINATION ${ophidian_placement_install_include_dir}
)
