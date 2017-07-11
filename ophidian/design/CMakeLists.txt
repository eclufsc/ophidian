file(GLOB ophidian_design_SRC
    "*.cpp"
)

add_library(ophidian_design ${ophidian_design_SRC})
target_include_directories(ophidian_design PUBLIC ${THIRD_PARTY_SOURCE_DIR} )
target_link_libraries(ophidian_design PUBLIC 
    ophidian_circuit 
    ophidian_floorplan 
    ophidian_standard_cell 
    ophidian_placement 
    ophidian_parser
)

install(TARGETS ophidian_design DESTINATION lib)
install(FILES Design.h DesignBuilder.h DESTINATION include/ophidian/design)
