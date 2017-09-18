################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::circuit
#
# Its main goals are:
#   - Fetch library files.
#   - Add compilation target.
#   - Add target specific include paths.
#   - Add compilation target dependencies. 
#       `-(this is a must due to telling cmake what should it compile fisrt).
#   - Define installation parameters.
################################################################################

# Fetch .ccp files for library creation
file(GLOB ophidian_circuit_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_circuit ${ophidian_circuit_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_circuit PUBLIC 
    ophidian_entity_system 
    ophidian_parser 
    ophidian_standard_cell
)

# Instal parameters for make install
install(TARGETS ophidian_circuit DESTINATION lib)
install(FILES Netlist.h DESTINATION include/ophidian/circuit)
