file(GLOB ophidian_circuit_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_circuit ${ophidian_circuit_SRC})
target_link_libraries(ophidian_circuit PUBLIC verilogparser)
install(TARGETS ophidian_circuit DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/circuit)
