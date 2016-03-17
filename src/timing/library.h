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

class library {
	openeda::standard_cell::standard_cells & m_std_cells;
	entity::vector_property<boost::units::quantity<boost::units::si::capacitance> > m_pin_capacitance;

	library_timing_arcs m_tarcs;

public:
	library(openeda::standard_cell::standard_cells * std_cells);
	virtual ~library();
	void pin_capacitance(entity::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance);
	boost::units::quantity<boost::units::si::capacitance> pin_capacitance(entity::entity pin) const {
		return m_pin_capacitance[m_std_cells.pin_system().lookup(pin)];
	}
	openeda::standard_cell::standard_cells & std_cells() {
		return m_std_cells;
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

	void timing_arc_rise_slew(entity::entity arc, const library_timing_arcs::LUT & lut);
	void timing_arc_fall_slew(entity::entity arc, const library_timing_arcs::LUT & lut);
	void timing_arc_rise_delay(entity::entity arc, const library_timing_arcs::LUT & lut);
	void timing_arc_fall_delay(entity::entity arc, const library_timing_arcs::LUT & lut);

	const library_timing_arcs::LUT & timing_arc_rise_slew(entity::entity arc) const {
		return m_tarcs.rise_slew(arc);
	}
	const library_timing_arcs::LUT & timing_arc_fall_slew(entity::entity arc) const {
		return m_tarcs.fall_slew(arc);
	}
	const library_timing_arcs::LUT & timing_arc_rise_delay(entity::entity arc) const {
		return m_tarcs.rise_delay(arc);
	}
	const library_timing_arcs::LUT & timing_arc_fall_delay(entity::entity arc) const {
		return m_tarcs.fall_delay(arc);
	}
	entity::entity timing_arc(entity::entity from, entity::entity to) const {
		return m_tarcs.get(from, to);
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_LIBRARY_H_ */
