/*
 * library.h
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_LIBRARY_H_
#define SRC_TIMING_LIBRARY_H_

#include "../standard_cell/standard_cells.h"
#include "../entity/vector_property.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include "library_timing_arcs.h"

namespace openeda {
namespace timing {

enum class unateness {
	NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE
};

class library {
public:
	using LUT = lookup_table<boost::units::quantity<boost::units::si::capacitance>, boost::units::quantity<boost::units::si::time>, boost::units::quantity<boost::units::si::time>>;
private:
	openeda::standard_cell::standard_cells & m_std_cells;
	library_timing_arcs & m_tarcs;

	entity::vector_property<LUT> m_rise_delays;
	entity::vector_property<LUT> m_fall_delays;
	entity::vector_property<LUT> m_rise_slews;
	entity::vector_property<LUT> m_fall_slews;
	entity::vector_property<unateness> m_timing_senses;
	entity::vector_property<boost::units::quantity<boost::units::si::capacitance> > m_pin_capacitance;

public:
	library(library_timing_arcs * tarcs, openeda::standard_cell::standard_cells * std_cells);
	virtual ~library();
	void pin_capacitance(entity::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance);
	boost::units::quantity<boost::units::si::capacitance> pin_capacitance(entity::entity pin) const {
		return m_pin_capacitance[m_std_cells.pin_system().lookup(pin)];
	}
	openeda::standard_cell::standard_cells & std_cells() {
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
} /* namespace openeda */

#endif /* SRC_TIMING_LIBRARY_H_ */
