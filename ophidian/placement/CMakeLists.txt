file(GLOB ophidian_placement_SRC
    "*.cpp"
)

add_library(ophidian_placement ${ophidian_placement_SRC})
target_include_directories(ophidian_placement PUBLIC ${THIRD_PARTY_SOURCE_DIR} )
target_link_libraries(ophidian_placement PUBLIC
    ophidian_parser
    ophidian_circuit
    ophidian_entitysystem
    ophidian_standard_cell
    ophidian_geometry
)

install(TARGETS ophidian_placement DESTINATION lib)
install(FILES Placement.h Library.h DESTINATION include/ophidian/placement)
