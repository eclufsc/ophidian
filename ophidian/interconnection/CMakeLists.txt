file(GLOB ophidian_interconnection_SRC
    "*.cpp"
)

add_library(ophidian_interconnection ${ophidian_interconnection_SRC})
target_include_directories(ophidian_interconnection PUBLIC ${Boost_INCLUDE_DIRS} ${THIRD_PARTY_SOURCE_DIR})
target_link_libraries(ophidian_interconnection PUBLIC 
    flute 
    pthread 
    emon
    ophidian_geometry
)

install(TARGETS ophidian_interconnection DESTINATION lib)
install(FILES Flute.h DESTINATION include/ophidian/interconnection)
