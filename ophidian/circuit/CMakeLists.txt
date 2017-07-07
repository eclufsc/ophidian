INCLUDE_DIRECTORIES( ${THIRD_PARTY_PATH} )
file(GLOB ophidian_circuit_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_circuit ${ophidian_circuit_SRC})
install(TARGETS ophidian_circuit DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/circuit)
