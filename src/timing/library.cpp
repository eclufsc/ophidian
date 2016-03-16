/*
 * library.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#include "library.h"

namespace openeda {
namespace timing {

library::library(openeda::standard_cell::standard_cells * std_cells):
				m_std_cells(*std_cells)
{
	std_cells->register_pin_property(&m_pin_capacitance);
}

library::~library() {
}

void library::pin_capacitance(entity::entity pin,
		boost::units::quantity<boost::units::si::capacitance> capacitance) {
	m_pin_capacitance[m_std_cells.pin_system().lookup(pin)] = capacitance;
}

} /* namespace timing */
} /* namespace openeda */
