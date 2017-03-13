file(GLOB ophidian_standard_cell_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_standard_cell ${ophidian_standard_cell_SRC})
install(TARGETS ophidian_standard_cell DESTINATION lib)
install(FILES Pins.h Cells.h StandardCell.h DESTINATION include/ophidian/standard_cell)
