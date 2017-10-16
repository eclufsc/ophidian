################################################################################
# This is the CMakeLists file for the Ophidian Dependencies. 
# Its main goals are:
#   - Fetch dependencies 
#   - Configure dependencies
#   - Add dependencies subdirectories
################################################################################

# Init submodules and fetch dependences from git
execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE submodule_init_exit_code
)

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
ExternalProject_Add(
	lef_parser
    SOURCE_DIR ${third_party_source_dir}/LEF
	# Empty CONFIGURE_COMMAND as the LEF parser has no configure file
	CONFIGURE_COMMAND ""
    BUILD_COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
	# LEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

# Build DEF parser with the included Makefile
ExternalProject_Add(
	def_parser
    SOURCE_DIR ${third_party_source_dir}/DEF
	# Empty CONFIGURE_COMMAND as the DEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
	# DEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)
