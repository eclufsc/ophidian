/*
 * cells.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#include <cells.h>

namespace openeda {
namespace standard_cell {

cells::cells(entity::system& system) :
		m_system(system) {
	system.register_property(&m_names);
	system.register_property(&m_pins);
}

cells::~cells() {
}

void cells::name(entity::entity e, std::string name) {
	m_names[m_system.lookup(e)] = name;
}

void cells::insert_pin(entity::entity cell, entity::entity pin) {
	m_pins[m_system.lookup(cell)].push_back(pin);
}

void cells::pins(entity::entity cell, std::vector<entity::entity> pins) {
	m_pins[m_system.lookup(cell)] = pins;
}

} /* namespace standard_cell */
} /* namespace openeda */
