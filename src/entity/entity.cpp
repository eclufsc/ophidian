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
	std::size_t property_index = m_entities.size();
	m_entities.insert(boost::bimap<entity, std::size_t>::value_type(new_entity, property_index));
	for(auto p : m_properties) {
		p->create(new_entity, property_index);
	}
	return id;
}

void system::destroy(entity e) {
	std::size_t index = m_entities.left.at(e);
	std::size_t last_index = m_entities.size() - 1;
	entity last_entity = m_entities.right.at(last_index);
	m_entities.left.erase(e);
	if (last_index != index) {
		m_entities.right.erase(last_index);
		m_entities.insert(boost::bimap<entity, std::size_t>::value_type(last_entity, index));
	}
	for(auto p : m_properties)
		p->destroy(e, index);
}

void system::register_property(property* p) {
	m_properties.insert(p);
}

} /* namespace entity */
} /* namespace openeda */

