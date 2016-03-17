/*
 * library_timing_arcs.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: csguth
 */

#include "library_timing_arcs.h"

namespace openeda {
namespace timing {

library_timing_arcs::library_timing_arcs(
		standard_cell::standard_cells * std_cells) :
		m_std_cells(*std_cells) {

	m_system.register_property(&m_from);
	m_system.register_property(&m_to);
	m_system.register_property(&m_rise_delays);
	m_system.register_property(&m_fall_delays);
	m_system.register_property(&m_rise_slews);
	m_system.register_property(&m_fall_slews);

	m_std_cells.register_pin_property(&m_pin_timing_arcs);
}

library_timing_arcs::~library_timing_arcs() {
}

entity::entity library_timing_arcs::create(entity::entity from,
		entity::entity to) {
	auto pair = std::make_pair(from, to);
	auto result = m_pinpair2arc.find(pair);
	if(result != m_pinpair2arc.end())
		return result->second;

	entity::entity arc = m_system.create();
	m_pinpair2arc[pair] = arc;
	m_from[m_system.lookup(arc)] = from;
	m_to[m_system.lookup(arc)] = to;
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(from)].push_back(arc);
	m_pin_timing_arcs[m_std_cells.pin_system().lookup(to)].push_back(arc);
	return arc;
}

entity::entity library_timing_arcs::get(entity::entity from,
		entity::entity to) const {
	return m_pinpair2arc.at(std::make_pair(from, to));
}


void library_timing_arcs::rise_delay(entity::entity arc, const LUT& lut) {
	m_rise_delays[m_system.lookup(arc)] = lut;
}

void library_timing_arcs::fall_delay(entity::entity arc, const LUT& lut) {
	m_fall_delays[m_system.lookup(arc)] = lut;
}

void library_timing_arcs::rise_slew(entity::entity arc, const LUT& lut) {
	m_rise_slews[m_system.lookup(arc)] = lut;
}

void library_timing_arcs::fall_slew(entity::entity arc, const LUT& lut) {
	m_fall_slews[m_system.lookup(arc)] = lut;
}

} /* namespace timing */
} /* namespace openeda */

