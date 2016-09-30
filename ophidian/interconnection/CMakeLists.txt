file(GLOB ophidian_interconnection_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_interconnection ${ophidian_interconnection_SRC})
target_include_directories(ophidian_interconnection PUBLIC ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/3rdparty)
target_link_libraries(ophidian_interconnection PUBLIC flute)
install(TARGETS ophidian_interconnection DESTINATION lib)
install(FILES Flute.h DESTINATION include/ophidian/interconnection)
