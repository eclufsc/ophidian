/*
 * nets.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "nets.h"

namespace openeda {
namespace netlist {

nets::nets(entity::system * entity_system) {
	entity_system->register_property(this);
}

nets::~nets() {
}

void nets::create(entity::entity e) {
	m_names.resize(m_names.size() + 1);
	m_pins.resize(m_pins.size() + 1);
	m_entity2index[e] = m_index2entity.size();
	m_index2entity.push_back(e);
}

void nets::destroy(entity::entity e) {
	std::size_t to_destroy = m_entity2index[e];
	std::size_t last = m_index2entity.size() - 1;
	m_names[to_destroy] = m_names[last];
	m_pins[to_destroy] = m_pins[last];
	m_index2entity[to_destroy] = m_index2entity[last];
	m_entity2index[m_index2entity[to_destroy]] = to_destroy;
	m_names.pop_back();
	m_pins.pop_back();
	m_index2entity.pop_back();
	m_entity2index.erase(e);
}

void nets::connect(entity::entity net, entity::entity pin) {
	m_pins[m_entity2index[net]].push_back(pin);
}

void nets::disconnect(entity::entity net, entity::entity pin) {
	auto & pins_vector = m_pins[m_entity2index.at(net)];
	std::size_t i = 0;
	while(i < pins_vector.size())
	{
		if(pins_vector[i] == pin)
		{
			pins_vector[i] = pins_vector.back();
			pins_vector.pop_back();
			return;
		}
		++i;
	}
	throw std::runtime_error("pin already disconnected");
}

void nets::pins(entity::entity net, std::vector<entity::entity> pins) {
	m_pins[m_entity2index.at(net)] = pins;
}

void nets::name(entity::entity net, std::string name) {
	m_names[m_entity2index.at(net)] = name;
}

} /* namespace netlist */
} /* namespace openeda */
