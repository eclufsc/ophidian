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
				m_std_cells(*std_cells),
				m_tarcs(std_cells)
{
	std_cells->register_pin_property(&m_pin_capacitance);
}

library::~library() {
}

void library::pin_capacitance(entity::entity pin,
		boost::units::quantity<boost::units::si::capacitance> capacitance) {
	m_pin_capacitance[m_std_cells.pin_system().lookup(pin)] = capacitance;
}

void library::timing_arc_rise_slew(entity::entity arc,
		const library_timing_arcs::LUT& lut) {
	m_tarcs.rise_slew(arc, lut);
}

void library::timing_arc_fall_slew(entity::entity arc,
		const library_timing_arcs::LUT& lut) {
	m_tarcs.fall_slew(arc, lut);
}

void library::timing_arc_rise_delay(entity::entity arc,
		const library_timing_arcs::LUT& lut) {
	m_tarcs.rise_delay(arc, lut);
}

void library::timing_arc_fall_delay(entity::entity arc,
		const library_timing_arcs::LUT& lut) {
	m_tarcs.fall_delay(arc, lut);
}

} /* namespace timing */
} /* namespace openeda */
