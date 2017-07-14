file(GLOB ophidian_circuit_SRC
    "*.cpp"
)

add_library(ophidian_circuit ${ophidian_circuit_SRC})
target_include_directories( ophidian_circuit PUBLIC ${THIRD_PARTY_SOURCE_DIR} )
target_link_libraries(ophidian_circuit PUBLIC 
    ophidian_entitysystem 
    ophidian_parser 
    ophidian_standard_cell
)

install(TARGETS ophidian_circuit DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/circuit)
