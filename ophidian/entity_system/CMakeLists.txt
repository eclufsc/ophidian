aux_source_directory(. SRC_LIST)
add_library(ophidian_entitysystem ${SRC_LIST})
install(TARGETS ophidian_entitysystem DESTINATION lib)
install(FILES EntitySystem.h Property.h DESTINATION include/ophidian/entity_system)
