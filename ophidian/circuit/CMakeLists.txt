file(GLOB ophidian_circuit_SRC
    "*.cpp"
)

add_library(ophidian_circuit ${ophidian_circuit_SRC})
target_include_directories( ophidian_circuit PUBLIC ${THIRD_PARTY_SOURCE_DIR} )

install(TARGETS ophidian_circuit DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/circuit)
