/*
 * library.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#include "library.h"

namespace openeda {
namespace timing {

library::library(library_timing_arcs * tarcs, openeda::standard_cell::standard_cells * std_cells) :
		m_std_cells(*std_cells), m_tarcs(*tarcs) {
	std_cells->register_pin_property(&m_pin_capacitance);
	m_tarcs.register_property(&m_rise_delays);
	m_tarcs.register_property(&m_rise_slews);
	m_tarcs.register_property(&m_fall_delays);
	m_tarcs.register_property(&m_fall_slews);
	m_tarcs.register_property(&m_timing_senses);
}

library::~library() {
}

void library::pin_capacitance(entity::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance) {
	m_pin_capacitance[m_std_cells.pin_system().lookup(pin)] = capacitance;
}

void library::timing_arc_rise_slew(entity::entity arc, const LUT& lut) {
	m_rise_slews[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_fall_slew(entity::entity arc, const LUT& lut) {
	m_fall_slews[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_rise_delay(entity::entity arc, const LUT& lut) {
	m_rise_delays[m_tarcs.system().lookup(arc)] = lut;
}

void library::timing_arc_fall_delay(entity::entity arc, const LUT& lut) {
	m_fall_delays[m_tarcs.system().lookup(arc)] = lut;
}

entity::entity library::cell_create(std::string name) {
	return m_std_cells.cell_create(name);
}

entity::entity library::pin_create(entity::entity cell, std::string name) {
	return m_std_cells.pin_create(cell, name);
}

void library::timing_arc_timing_sense(entity::entity arc, unateness timing_sense) {
	m_timing_senses[m_tarcs.system().lookup(arc)] = timing_sense;
}

} /* namespace timing */
} /* namespace openeda */

