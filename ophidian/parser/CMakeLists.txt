find_package( Boost 1.59 )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} ${THIRD_PARTY_PATH})

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)

set(PARSER_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/Lef.cpp
)

add_library (ophidian_parser ${PARSER_SOURCE} )

target_include_directories ( ophidian_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${THIRD_PARTY_PATH})
target_link_libraries( ophidian_parser ${Boost_LIBRARIES} lef )
