################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::standard_cell
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
file(GLOB ophidian_standard_cell_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_standard_cell ${ophidian_standard_cell_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_standard_cell PUBLIC
    ophidian_entity_system
)

# Instal parameters for make install
install(TARGETS ophidian_standard_cell DESTINATION lib)
install(FILES Pins.h Cells.h StandardCell.h DESTINATION include/ophidian/standard_cell)
