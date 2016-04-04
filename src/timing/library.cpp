/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "library.h"

namespace ophidian {
namespace timing {

library::library(library_timing_arcs * tarcs, ophidian::standard_cell::standard_cells * std_cells) :
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

