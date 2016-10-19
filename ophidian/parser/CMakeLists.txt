INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_parser_SRC
    "*.h"
    "*.cpp"
)

set(PARSER_SOURCE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Def.cpp)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

add_library(ophidian_parser ${ophidian_parser_SRC})

target_include_directories( ophidian_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH} )
target_link_libraries(ophidian_parser PUBLIC def)
target_link_libraries(ophidian_parser PUBLIC verilogparser)

install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h DESTINATION include/ophidian/parser)
