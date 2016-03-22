/*
 * sta_timing_point_calculator.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "sta_timing_point_calculator.h"

#include <cmath>

namespace openeda {
namespace timing {

sta_timing_point_calculator::sta_timing_point_calculator() {

}

sta_timing_point_calculator::~sta_timing_point_calculator() {
}

void sta_timing_point_calculator::update(const lemon::ListDigraph& graph, const graph_arcs_timing & arcs_timing, graph_nodes_timing& nodes_timing, lemon::ListDigraph::Node node) {

	using namespace boost::units;
	quantity<si::time> arrival( std::numeric_limits<double>::lowest() * si::second );
	for(lemon::ListDigraph::InArcIt it( graph, node ); it != lemon::INVALID; ++it)
	{
		lemon::ListDigraph::Node from( graph.source(static_cast<lemon::ListDigraph::Arc>(it)) );
		arrival = std::max( arrival, nodes_timing.arrival(from) + arcs_timing.delay(static_cast<lemon::ListDigraph::Arc>(it)) );
	}

	nodes_timing.arrival(node, arrival);

}

} /* namespace timing */
} /* namespace openeda */
