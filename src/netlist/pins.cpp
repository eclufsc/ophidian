/*
 * pins.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: csguth
 */

#include "pins.h"

namespace openeda {
namespace netlist {

pins::pins(entity::system & system) : m_system(system) {
	m_system.register_property(&m_owners);
	m_system.register_property(&m_nets);
	m_system.register_property(&m_names);
}

pins::~pins() {
}

void pins::owner(entity::entity pin, entity::entity owner) {
	m_owners[m_system.lookup(pin)] = owner;
}

void pins::net(entity::entity pin, entity::entity net) {
	m_nets[m_system.lookup(pin)] = net;
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

} /* namespace netlist */
} /* namespace openeda */

