/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "entity.h"

namespace ophidian {
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
    for(auto it = m_entities.left.begin(); it != m_entities.left.end(); ++it)
    {
        entity e = it->first;
        std::size_t index = it->second;
        p->create(e, index);
    }


}

} /* namespace entity */
} /* namespace ophidian */

