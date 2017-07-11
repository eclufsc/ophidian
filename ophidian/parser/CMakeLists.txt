file(GLOB ophidian_parser_SRC
    "*.cpp"
)

link_directories(${THIRD_PARTY_SOURCE_DIR}/DEF/lib)
link_directories(${THIRD_PARTY_SOURCE_DIR}/LEF/lib)

add_library(ophidian_parser ${ophidian_parser_SRC})
target_include_directories( ophidian_parser PUBLIC ${THIRD_PARTY_SOURCE_DIR} )
target_link_libraries(ophidian_parser PUBLIC 
    verilogparser
)

install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h DESTINATION include/ophidian/parser)
