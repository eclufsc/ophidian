################################################################################
# This is the CMakeLists file for the:
#
#   namespace ophidian::entity_system
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
file(GLOB ophidian_entity_system_SRC
    "*.cpp"
)

# Add library target
add_library(ophidian_entity_system ${ophidian_entity_system_SRC})

# Tell cmake target's dependencies
target_link_libraries(ophidian_entity_system PUBLIC 
    lemon
)

# Instal parameters for make install
install(TARGETS ophidian_entity_system DESTINATION lib)
install(FILES EntitySystem.h Property.h DESTINATION include/ophidian/entity_system)
