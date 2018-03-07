################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::timing 
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
file(GLOB ophidian_timingdriven_placement_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_timingdriven_placement ${ophidian_timingdriven_placement_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_timingdriven_placement PUBLIC
    flute
    lemon
    ophidian_geometry
)

# Instal parameters for make install
install(TARGETS ophidian_timingdriven_placement DESTINATION lib)
#install(FILES RCTree.h DESTINATION include/ophidian/timingdriven_placement)
