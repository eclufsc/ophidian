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
	m_system.register_property(&m_std_cell_pin);
}

pins::~pins() {
}

void pins::owner(entity::entity pin, entity::entity owner) {
	m_owners[m_system.lookup(pin)] = owner;
}

void pins::net(entity::entity pin, entity::entity net) {
	m_nets[m_system.lookup(pin)] = net;
}

void pins::standard_cell_pin(entity::entity pin,
		entity::entity std_cell_pin) {
	m_std_cell_pin[m_system.lookup(pin)] = std_cell_pin;
}

} /* namespace netlist */
} /* namespace openeda */

