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

#ifndef SRC_TIMING_LIBRARY_H_
#define SRC_TIMING_LIBRARY_H_

#include "../standard_cell/standard_cells.h"
#include "../entity/vector_property.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include "library_timing_arcs.h"

namespace ophidian {
namespace timing {

enum class unateness {
	NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE
};

class library {
public:
	using LUT = lookup_table<boost::units::quantity<boost::units::si::capacitance>, boost::units::quantity<boost::units::si::time>, boost::units::quantity<boost::units::si::time>>;
private:
	ophidian::standard_cell::standard_cells & m_std_cells;
	library_timing_arcs & m_tarcs;

	entity::vector_property<LUT> m_rise_delays;
	entity::vector_property<LUT> m_fall_delays;
	entity::vector_property<LUT> m_rise_slews;
	entity::vector_property<LUT> m_fall_slews;
	entity::vector_property<unateness> m_timing_senses;
	entity::vector_property<boost::units::quantity<boost::units::si::capacitance> > m_pin_capacitance;

public:
	library(library_timing_arcs * tarcs, ophidian::standard_cell::standard_cells * std_cells);
	virtual ~library();
	void pin_capacitance(entity::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance);
	boost::units::quantity<boost::units::si::capacitance> pin_capacitance(entity::entity pin) const {
		return m_pin_capacitance[m_std_cells.pin_system().lookup(pin)];
	}
	ophidian::standard_cell::standard_cells & std_cells() {
		return m_std_cells;
	}

	standard_cell::pin_directions pin_direction(entity::entity pin) const {
		return m_std_cells.pin_direction(pin);
	}

	entity::entity timing_arc_from(entity::entity arc) const {
		return m_tarcs.from(arc);
	}

	entity::entity timing_arc_to(entity::entity arc) const {
		return m_tarcs.to(arc);
	}

	entity::entity timing_arc_create(entity::entity from, entity::entity to) {
		return m_tarcs.create(from, to);
	}

	const std::vector<entity::entity>& pin_timing_arcs(entity::entity pin) const {
		return m_tarcs.pin_timing_arcs(pin);
	}

	void timing_arc_rise_slew(entity::entity arc, const LUT & lut);
	void timing_arc_fall_slew(entity::entity arc, const LUT & lut);
	void timing_arc_rise_delay(entity::entity arc, const LUT & lut);
	void timing_arc_fall_delay(entity::entity arc, const LUT & lut);

	const LUT & timing_arc_rise_slew(entity::entity arc) const {
		return m_rise_slews[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_fall_slew(entity::entity arc) const {
		return m_fall_slews[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_rise_delay(entity::entity arc) const {
		return m_rise_delays[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_fall_delay(entity::entity arc) const {
		return m_fall_delays[m_tarcs.system().lookup(arc)];
	}
	entity::entity timing_arc(entity::entity from, entity::entity to) const {
		return m_tarcs.get(from, to);
	}


	void timing_arc_timing_sense(entity::entity arc, unateness timing_sense);
	unateness timing_arc_timing_sense(entity::entity arc) const {
		return m_timing_senses[m_tarcs.system().lookup(arc)];
	}


	entity::entity cell_create(std::string name);
	entity::entity pin_create(entity::entity cell, std::string name);
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_LIBRARY_H_ */
