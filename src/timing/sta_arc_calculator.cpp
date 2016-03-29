/*
 * sta_arc_calculator.cpp
 *
 *  Created on: Mar 24, 2016
 *      Author: csguth
 */
#include "sta_arc_calculator.h"
#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

namespace openeda {
namespace timing {

void sta_arc_calculator::update(const graph& g, const graph_nodes_timing& nodes_timing, sta_timing_point_calculator & tpoints, graph_arcs_timing & m_arcs) {
	for (lemon::ListDigraph::Arc arc : m_to_process) {
		auto source_slew = nodes_timing.slew(g.edge_source(arc));
		auto elmore_slew = m_arcs.slew(arc);
		// extend to ramp slew
		m_arcs.slew(arc, boost::units::sqrt(boost::units::pow<2>(elmore_slew) + boost::units::pow<2>(source_slew)));
	}
	// CRITICAL >>>>
	for (lemon::ListDigraph::Arc arc : m_to_process) {
		tpoints.push(g.edge_target(arc));
	}
	m_to_process.clear();
	// <<<<
}
}
}



