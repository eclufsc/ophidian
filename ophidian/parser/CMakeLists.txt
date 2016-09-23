file(GLOB ophidian_parser_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_parser ${ophidian_parser_SRC})
link_directories( 3rdparty/liberty-parser/lib/ )
target_link_libraries(ophidian_parser PUBLIC verilogparser ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/liberty-parser/lib/libsi2dr_liberty.a)
install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h LibertyParser.h DESTINATION include/ophidian/parser)
