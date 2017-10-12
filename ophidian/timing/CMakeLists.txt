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
file(GLOB ophidian_timing_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_timing ${ophidian_timing_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_timing PUBLIC 
    OpenTimer
)

# Instal parameters for make install
install(TARGETS ophidian_timing DESTINATION lib)
install(FILES OpenTimer.h DESTINATION include/ophidian/timer)
