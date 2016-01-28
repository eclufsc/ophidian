/*
 * entity.cpp
 *
 *  Created on: Jan 27, 2016
 *      Author: csguth
 */

#include "entity.h"

namespace openeda {
namespace entity {

entity::entity(uint32_t id) :
		m_id(id) {

}

system::system() :
		m_next(0) {
}

entity system::create() {
	uint32_t id = m_next++;
	entity new_entity(id);
	m_entities[id] = new_entity;
	for(auto p : m_properties)
		p->create(new_entity);
	return id;
}

void system::destroy(entity e) {
	m_entities.erase(e.id());
	for(auto p : m_properties)
		p->destroy(e);
}

void system::register_property(property* p) {
	m_properties.insert(p);
}

} /* namespace entity */
} /* namespace openeda */

