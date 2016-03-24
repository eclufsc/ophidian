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

sta_timing_point_calculator::sta_timing_point_calculator(const graph& graph, const graph_arcs_timing& arcs_timing, graph_nodes_timing& nodes_timing) :
						m_graph(graph), m_arcs_timing(arcs_timing), m_nodes_timing(nodes_timing) {
}

sta_timing_point_calculator::~sta_timing_point_calculator() {
}

void sta_timing_point_calculator::push(lemon::ListDigraph::Node node) {
	m_to_process.push_back(node);
}

void sta_timing_point_calculator::process_queue(lemon::ListDigraph::ArcMap< sta_arc_calculator * > & tarcs) {
	using namespace boost::units;

	// PARALEL >>>>
	for (auto node : m_to_process) {
		quantity<si::time> arrival(std::numeric_limits<double>::lowest() * si::second);
		quantity<si::time> slew(std::numeric_limits<double>::lowest() * si::second);
		for (lemon::ListDigraph::InArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it) {
			lemon::ListDigraph::Node from(m_graph.edge_source(static_cast<lemon::ListDigraph::Arc>(it)));
			arrival = std::max(arrival, m_nodes_timing.arrival(from) + m_arcs_timing.delay(static_cast<lemon::ListDigraph::Arc>(it)));
			slew = std::max(slew, m_arcs_timing.slew(static_cast<lemon::ListDigraph::Arc>(it)));
		}
		m_nodes_timing.arrival(node, arrival);
		m_nodes_timing.slew(node, slew);
	}
	// <<<<
	// CRITICAL >>>>
	for (auto node : m_to_process) {
		for (lemon::ListDigraph::OutArcIt it(m_graph.G(), node); it != lemon::INVALID; ++it) {
			tarcs[it]->push(it);
		}
	}
	/// <<<<
	m_to_process.clear();

}

bool sta_timing_point_calculator::empty() {
	return m_to_process.empty();
}

} /* namespace timing */
} /* namespace openeda */

