/*
 * pins.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#include <pins.h>

namespace openeda {
namespace standard_cell {

pins::pins(entity::system & system) :
	m_system(system){
	m_system.register_property(&m_names);
	m_system.register_property(&m_owners);
	m_system.register_property(&m_directions);
}

pins::~pins() {
}

void pins::name(entity::entity pin, std::string name) {
	m_names[m_system.lookup(pin)] = name;
}

void pins::owner(entity::entity pin, entity::entity cell) {
	m_owners[m_system.lookup(pin)] = cell;
}

void pins::direction(entity::entity pin, pin_directions direction) {
	m_directions[m_system.lookup(pin)] = direction;
}

} /* namespace standard_cell */
} /* namespace openeda */

