################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::interconnection
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
file(GLOB ophidian_interconnection_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_interconnection ${ophidian_interconnection_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_interconnection PUBLIC 
    flute 
    pthread 
    lemon
    ophidian_geometry
)

# Instal parameters for make install
install(TARGETS ophidian_interconnection DESTINATION lib)
install(FILES Flute.h DESTINATION include/ophidian/interconnection)
