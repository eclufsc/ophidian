add_subdirectory(Lemon)
add_subdirectory(verilog-parser)
add_subdirectory(Flute)
add_subdirectory(OpenTimer)

include(ExternalProject)

# Build LEF v5.8-p016 parser with the included Makefile
ExternalProject_Add(
	lef_parser
    SOURCE_DIR ${THIRD_PARTY_SOURCE_DIR}/LEF
	# Empty CONFIGURE_COMMAND as the LEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# LEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)

# Build DEF v5.8-p016 parser with the included Makefile
ExternalProject_Add(
	def_parser
    SOURCE_DIR ${THIRD_PARTY_SOURCE_DIR}/DEF
	# Empty CONFIGURE_COMMAND as the DEF parser has no configure file
	CONFIGURE_COMMAND ""
	BUILD_COMMAND make installhdrs installlib
	# DEF Makefile is not compatible with external builds
	BUILD_IN_SOURCE 1
)
