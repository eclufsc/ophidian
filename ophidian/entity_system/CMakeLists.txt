aux_source_directory(. SRC_LIST)
add_library(ophidian_entitysystem ${SRC_LIST})
install(TARGETS ophidian_entitysystem DESTINATION lib)
install(FILES aggregation.h association.h attached_property.h composition.h entity_system.h property.h DESTINATION include/ophidian/entity_system)
