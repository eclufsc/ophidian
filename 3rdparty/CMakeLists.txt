################################################################################
# This is the CMakeLists file for the Ophidian Dependencies. 
# Its main goals are:
#   - Check for dependencies 
#   - Fetch dependencies 
#   - Configure dependencies
#   - Add dependencies subdirectories
################################################################################

# Init submodules and fetch dependences from git
#execute_process(
#    COMMAND git submodule update --init --recursive
#    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#    RESULT_VARIABLE submodule_init_exit_code
#)

# This is needed since Flute is statcly built
add_subdirectory(Flute)

# Add Lemon graph library
add_subdirectory(Lemon)

# Add OpenTimer
add_subdirectory(OpenTimer)

# Add verilog-parser
add_subdirectory(verilog-parser)

# Load ExternalProject cmake module
include(ExternalProject)

# Build LEF parser with the included Makefile
execute_process(
    COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
    WORKING_DIRECTORY ${third_party_source_dir}/LEF
)

add_library(LEF STATIC IMPORTED)
set_property(TARGET LEF PROPERTY IMPORTED_LOCATION LEF/lib/liblef.a)
set_property(TARGET LEF PROPERTY INTERFACE_INCLUDE_DIRECTORIES  
    $<BUILD_INTERFACE:      .>
    $<INSTALL_INTERFACE:    include/LEF>
)
# Build DEF parser with the included Makefile
execute_process(
    COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
    WORKING_DIRECTORY ${third_party_source_dir}/DEF
)

add_library(DEF STATIC IMPORTED)
set_property(TARGET DEF PROPERTY IMPORTED_LOCATION DEF/lib/libdef.a)
set_property(TARGET DEF PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    $<BUILD_INTERFACE:      .>
    $<INSTALL_INTERFACE:    include/DEF>
)
