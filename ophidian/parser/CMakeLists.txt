find_package( Boost 1.59 )
INCLUDE_DIRECTORIES(${THIRD_PARTY_PATH})

file(GLOB ophidian_parser_SRC
    "*.h"
    "*.cpp"
)

LINK_DIRECTORIES(${THIRD_PARTY_PATH}/LEF/lib/)
add_library(ophidian_parser ${ophidian_parser_SRC})
target_include_directories (
	ophidian_parser PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${THIRD_PARTY_PATH}
	${Boost_INCLUDE_DIR}
)
target_link_libraries(ophidian_parser PUBLIC ${Boost_LIBRARIES} verilogparser lef)
install(TARGETS ophidian_parser DESTINATION lib)
install(FILES VerilogParser.h DESTINATION include/ophidian/parser)
