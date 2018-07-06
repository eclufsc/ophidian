################################################################################
# This is the CMakeLists file for the:
#
#   Ophidian GUI
#
# Its main goals are:
#   - Create an graphic user interface.
#
################################################################################

################################################################################
# Set flags
################################################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

################################################################################
# Fetch files
################################################################################

# Fetch .cpp files for executable creation
file(GLOB_RECURSE ophidian_gui_source
    "*.cpp"
)

# Fetch .ui files for executable creation
file(GLOB_RECURSE ophidian_gui_userinterface
    "*.ui"
)

# Fetch .cpp files for executable creation
file(GLOB_RECURSE ophidian_gui_headers
    "*.h"
)

################################################################################
# Uncrustify
################################################################################

set(uncrustify_files ${ophidian_gui_source} ${ophidian_gui_userinterface} ${ophidian_gui_headers})

if(UNCRUSTIFY_IT)
    include(uncrustify_helper)
    uncrustify_it(${ophidian_uncrustify_config} "${uncrustify_files}")
endif()

if(RUN_UNCRUSTIFY_CHECK)
    include(uncrustify_helper)
    uncrustify_check(${ophidian_uncrustify_config} "${uncrustify_files}")
endif()

################################################################################
# GUI executable
################################################################################

# Add gui executable
add_executable( ophidian_gui ${ophidian_gui_source} ${ophidian_gui_userinterface} )

# Add executable dependencies
target_link_libraries(ophidian_gui PUBLIC
    PRIVATE Qt5::Widgets
    PRIVATE SFML::GRAPHICS
    PRIVATE SFML::WINDOW
    PRIVATE SFML::SYSTEM
    PRIVATE ophidian_circuit
    PRIVATE ophidian_design
    PRIVATE ophidian_entity_system
    PRIVATE ophidian_floorplan
    PRIVATE ophidian_geometry
    PRIVATE ophidian_interconnection
    PRIVATE ophidian_parser
    PRIVATE ophidian_placement
    PRIVATE ophidian_standard_cell
    PRIVATE ophidian_timing_static
    PRIVATE ophidian_timingdriven_placement_static
)

################################################################################
# Prepare gui input environment.
################################################################################

find_file(POWV9 "POWV9.dat" PATHS
    "/usr/share/Flute"
    "/usr/local/share/Flute"
    "${ophidian_source_dir}/dependencies/share/Flute"
)
find_file(PORT9 "PORT9.dat" PATHS
    "/usr/share/Flute"
    "/usr/local/share/Flute"
    "${ophidian_source_dir}/dependencies/share/Flute"
)

message("Creating symlink FLUTE LUTs for GUI")
execute_process(
    COMMAND ln -f -s ${POWV9} ${ophidian_install_bin_dir}/.
    COMMAND ln -f -s ${POWV9} ${CMAKE_CURRENT_BINARY_DIR}/.
    RESULT_VARIABLE result
)
execute_process(
    COMMAND ln -f -s ${POWV9} ${ophidian_install_bin_dir}/.
    COMMAND ln -f -s ${PORT9} ${CMAKE_CURRENT_BINARY_DIR}/.
    RESULT_VARIABLE result
)

################################################################################
# Installation rules
################################################################################

# Install rule for executable
install(
    TARGETS ophidian_gui
    DESTINATION ${ophidian_install_bin_dir}
)

