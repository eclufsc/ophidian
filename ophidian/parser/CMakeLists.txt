INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

file(GLOB ophidian_parser_SRC
    "*.h"
    "*.cpp"
)

set(PARSER_SOURCE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Def.cpp)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/DEF/lib/)

add_library(ophidian_parser ${ophidian_parser_SRC})

#link_directories( 3rdparty/libertyParser/lib/ )

target_include_directories( ophidian_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH} )
target_link_libraries(ophidian_parser PUBLIC def)
target_link_libraries(ophidian_parser PUBLIC verilogparser)
#target_link_libraries(ophidian_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/libertyParser/lib/libsi2dr_liberty.a)
#target_link_libraries(ophidian_parser PUBLIC si2dr_liberty)

install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h LibertyParser.h DESTINATION include/ophidian/parser)
