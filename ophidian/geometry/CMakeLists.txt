################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::geometry
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
file(GLOB ophidian_geometry_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_geometry ${ophidian_geometry_SRC})

# Instal parameters for make install
install(TARGETS ophidian_geometry DESTINATION lib)
install(FILES Distance.h Models.h Operations.h DESTINATION include/ophidian/geometry)
