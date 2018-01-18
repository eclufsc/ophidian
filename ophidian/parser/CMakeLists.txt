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

# Fetch .ccp files for library creation
file(GLOB ophidian_parser_SRC
    "*.cpp"
)

link_directories(${THIRD_PARTY_SOURCE_DIR}/DEF/lib)
link_directories(${THIRD_PARTY_SOURCE_DIR}/LEF/lib)
link_directories(${THIRD_PARTY_SOURCE_DIR}/libertyParser/lib/)

# Add library target
add_library(ophidian_parser ${ophidian_parser_SRC})

# Tell cmake target's dependencies
add_dependencies(ophidian_parser def_parser lef_parser)
target_link_libraries(ophidian_parser PUBLIC
    verilogparser
    si2dr_liberty
)

# Instal parameters for make install
install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h LibertyParser.h DESTINATION include/ophidian/parser)
