/*
 * nets.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "nets.h"

namespace openeda {
namespace netlist {

nets::nets() {
	register_property(&m_names);
	register_property(&m_pins);
}

nets::~nets() {
}

void nets::connect(entity::entity net, entity::entity pin) {
	m_pins[m_entities.left.at(net)].push_back(pin);
}

void nets::disconnect(entity::entity net, entity::entity pin) {
	auto & pins_vector = m_pins[m_entities.left.at(net)];
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
	m_pins[m_entities.left.at(net)] = pins;
}

void nets::name(entity::entity net, std::string name) {
	m_names[m_entities.left.at(net)] = name;
}

} /* namespace netlist */
} /* namespace openeda */
