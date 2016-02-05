/*
 * pins.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "pins.h"

namespace openeda {
namespace netlist {

pins::pins() {
	register_property(&m_owners);
	register_property(&m_nets);
	register_property(&m_names);
}

pins::~pins() {
}

void pins::owner(entity::entity pin, entity::entity owner) {
	m_owners[m_entities.left.at(pin)] = owner;
}

void pins::net(entity::entity pin, entity::entity net) {
	m_nets[m_entities.left.at(pin)] = net;
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_entities.left.at(pin)] = name;
}

} /* namespace netlist */
} /* namespace openeda */

