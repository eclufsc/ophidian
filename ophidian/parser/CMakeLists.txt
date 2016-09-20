INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )

set(PARSER_SOURCE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Def.cpp)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/def/lib/)

add_library( ophidian_parser ${PARSER_SOURCE} )

target_include_directories( ophidian_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH} )
target_link_libraries(ophidian_parser PUBLIC def)

install(TARGETS ophidian_parser DESTINATION lib)
