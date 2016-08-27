file(GLOB ophidian_entitysystem_SRC
    "*.h"
    "*.cpp"
)
add_library(ophidian_entitysystem ${ophidian_entitysystem_SRC})
install(TARGETS ophidian_entitysystem DESTINATION lib)
install(FILES EntitySystem.h Property.h DESTINATION include/ophidian/entity_system)
