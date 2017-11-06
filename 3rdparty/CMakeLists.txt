################################################################################
# This is the CMakeLists file for the Ophidian Build Dependencies directory.
# 
#   - Fetch dependencies.
#   - Configure dependencies.
#
################################################################################

# Init submodules and fetch dependences from git
execute_process(
COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE submodule_init_exit_code
)

# Add Catch
add_subdirectory(catch)

# Add Flute
add_subdirectory(Flute)

# Add verilog-parser
add_subdirectory(verilog-parser)

# Load ExternalProject cmake module
include(ExternalProject)

# Build LEF parser with the included Makefile
ExternalProject_Add(
	lef_parser
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/LEF
	CONFIGURE_COMMAND ""
    BUILD_COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
	BUILD_IN_SOURCE 1
)

add_library(LEF INTERFACE)
target_include_directories(LEF INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/LEF/include)
target_link_libraries(LEF INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/LEF/lib/liblef.a)

# Build DEF parser with the included Makefile
ExternalProject_Add(
	def_parser
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DEF
	CONFIGURE_COMMAND ""
    BUILD_COMMAND make CXXFLAGS="-fPIC" installhdrs installlib
	BUILD_IN_SOURCE 1
)

add_library(DEF INTERFACE)
target_include_directories(DEF INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DEF/include)
target_link_libraries(LEF INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/DEF/lib/libdef.a)
