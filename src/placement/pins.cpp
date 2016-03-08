/*
 * pins.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: csguth
 */

#include <pins.h>

namespace openeda {
namespace placement {

pins::pins(openeda::netlist::netlist* netlist) :
		m_system(netlist->pin_system()) {
	netlist->register_pin_property(&m_positions);
}

pins::~pins() {
}

void pins::position(entity::entity pin, point position_point) {
	m_positions[m_system.lookup(pin)] = position_point;
}

} /* namespace placement */
} /* namespace openeda */

