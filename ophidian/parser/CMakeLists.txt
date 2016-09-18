file(GLOB ophidian_parser_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_parser ${ophidian_parser_SRC})
target_link_libraries(ophidian_parser PUBLIC verilogparser)
install(TARGETS ophidian_parser DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/parser)
