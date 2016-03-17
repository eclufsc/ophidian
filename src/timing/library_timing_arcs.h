/*
 * library_timing_arcs.h
 *
 *  Created on: Mar 17, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_LIBRARY_TIMING_ARCS_H_
#define SRC_TIMING_LIBRARY_TIMING_ARCS_H_

#include "../entity/entity.h"
#include "../standard_cell/standard_cells.h"
#include "lookup_table.h"
#include <unordered_map>

namespace openeda {
namespace timing {

// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};

class library_timing_arcs {
public:
	using LUT = lookup_table<boost::units::quantity<boost::units::si::capacitance>, boost::units::quantity<boost::units::si::time>, boost::units::quantity<boost::units::si::time>>;
private:
	standard_cell::standard_cells & m_std_cells;

	entity::system m_system;
	entity::vector_property<entity::entity> m_from;
	entity::vector_property<entity::entity> m_to;

	entity::vector_property<LUT> m_rise_delays;
	entity::vector_property<LUT> m_fall_delays;
	entity::vector_property<LUT> m_rise_slews;
	entity::vector_property<LUT> m_fall_slews;

	//	// std_cell properties
		entity::vector_property< std::vector<entity::entity> > m_pin_timing_arcs;


		std::unordered_map< std::pair< entity::entity, entity::entity >, entity::entity,pair_hash > m_pinpair2arc;
public:
	library_timing_arcs(standard_cell::standard_cells * std_cells);
	virtual ~library_timing_arcs();
	entity::entity create(entity::entity from, entity::entity to);
	entity::entity get(entity::entity from, entity::entity to) const;

	entity::entity from(entity::entity arc) const {
		return m_from[m_system.lookup(arc)];
	}
	entity::entity to(entity::entity arc) const {
		return m_to[m_system.lookup(arc)];
	}

	std::size_t size() const {
		return m_system.size();
	}

	const entity::system & system() const {
		return m_system;
	}

	void rise_delay(entity::entity arc, const LUT & lut);
	const LUT & rise_delay(entity::entity arc) const {
		return m_rise_delays[m_system.lookup(arc)];
	}

	void fall_delay(entity::entity arc, const LUT & lut);
	const LUT & fall_delay(entity::entity arc) const {
		return m_fall_delays[m_system.lookup(arc)];
	}

	void rise_slew(entity::entity arc, const LUT & lut);
	const LUT & rise_slew(entity::entity arc) const {
		return m_rise_slews[m_system.lookup(arc)];
	}

	void fall_slew(entity::entity arc, const LUT & lut);
	const LUT & fall_slew(entity::entity arc) const {
		return m_fall_slews[m_system.lookup(arc)];
	}

	const std::vector<entity::entity> & pin_timing_arcs(entity::entity pin) const {
		return m_pin_timing_arcs[m_std_cells.pin_system().lookup(pin)];
	}

};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_LIBRARY_TIMING_ARCS_H_ */
