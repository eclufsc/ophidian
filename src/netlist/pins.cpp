/*
 * pins.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "pins.h"

namespace openeda {
namespace netlist {

pins::pins(entity::system * entity_system) {
	entity_system->register_property(this);
}

pins::~pins() {
}

void pins::create(entity::entity e) {
	m_names.resize(m_names.size() + 1);
	m_owners.resize(m_owners.size() + 1);
	m_nets.resize(m_nets.size() + 1);
	m_entity2index[e] = m_index2entity.size();
	m_index2entity.push_back(e);
}

void pins::destroy(entity::entity e) {
	std::size_t to_destroy = m_entity2index[e];
	std::size_t last = m_index2entity.size() - 1;
	m_names[to_destroy] = m_names[last];
	m_nets[to_destroy] = m_nets[last];
	m_owners[to_destroy] = m_owners[last];
	m_index2entity[to_destroy] = m_index2entity[last];
	m_entity2index[m_index2entity[to_destroy]] = to_destroy;
	m_names.pop_back();
	m_owners.pop_back();
	m_nets.pop_back();
	m_index2entity.pop_back();
	m_entity2index.erase(e);
}

void pins::owner(entity::entity pin, entity::entity owner) {
	m_owners[m_entity2index[pin]] = owner;
}

void pins::net(entity::entity pin, entity::entity net) {
	m_nets[m_entity2index[pin]] = net;
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_entity2index[pin]] = name;
}

} /* namespace netlist */
} /* namespace openeda */

