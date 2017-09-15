################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::design
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
file(GLOB ophidian_design_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_design ${ophidian_design_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_design PUBLIC 
    ophidian_circuit 
    ophidian_floorplan 
    ophidian_standard_cell 
    ophidian_placement 
    ophidian_parser
)

# Instal parameters for make install
install(TARGETS ophidian_design DESTINATION lib)
install(FILES Design.h DesignBuilder.h DESTINATION include/ophidian/design)
