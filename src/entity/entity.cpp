/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "entity.h"

namespace ophidian {
namespace entity {

entity::entity(uint32_t id) :
		m_id(id) {

}

void system::preallocate(std::size_t qnt)
{
    for(auto property : m_properties)
        property->preallocate(qnt);
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

