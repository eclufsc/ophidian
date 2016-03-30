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

    using namespace boost::units;
    using namespace boost::units::si;
    quantity<si::time> source_slew, elmore_slew;
    for (std::deque<lemon::ListDigraph::Arc>::const_iterator arc_it = m_to_process.begin(); arc_it != m_to_process.end(); ++arc_it) {
        source_slew = nodes_timing.slew(g.edge_source(*arc_it));
        elmore_slew = m_arcs.slew(*arc_it);
		// extend to ramp slew
        m_arcs.slew(*arc_it, boost::units::sqrt(boost::units::pow<2>(elmore_slew) + boost::units::pow<2>(source_slew)));
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



