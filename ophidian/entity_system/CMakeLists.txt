file(GLOB ophidian_entitysystem_SRC
    "*.cpp"
)

add_library(ophidian_entitysystem ${ophidian_entitysystem_SRC})
target_link_libraries(ophidian_entitysystem PUBLIC emon)

install(TARGETS ophidian_entitysystem DESTINATION lib)
install(FILES EntitySystem.h Property.h DESTINATION include/ophidian/entity_system)
