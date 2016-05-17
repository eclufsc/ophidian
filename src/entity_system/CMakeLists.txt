add_library (entity_system entity_system.cpp entity_system.h entity.h property.h vector_property.h)
target_include_directories (entity_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
