/*
 * static_timing_analysis.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#include "static_timing_analysis.h"

namespace openeda {
namespace timing {

static_timing_analysis::static_timing_analysis(const timing::graph & graph, const library & lib) :
		m_graph(graph), m_nodes(graph.G()), m_arcs(graph.G()), m_net_calculator(nullptr), m_timing_arc_calculator(m_graph, lib, m_nodes, m_arcs), m_timing_point_calculator(m_graph, m_arcs, m_nodes), m_arcs_calculators(m_graph.G()) {
}

static_timing_analysis::~static_timing_analysis() {
}

void static_timing_analysis::net_calculator(sta_net_calculator* net_calculator) {
	m_net_calculator = net_calculator;
}

void static_timing_analysis::make_dirty(entity::entity net) {
	m_net_calculator->make_dirty(net);
}

void static_timing_analysis::arc_calculator(lemon::ListDigraph::Arc arc, sta_arc_calculator* arc_calculator) {
	m_arcs_calculators[arc] = arc_calculator;
}

void static_timing_analysis::run() {
	m_net_calculator->update_dirty_nets(m_nodes, m_arcs);

	for (lemon::ListDigraph::NodeIt node(m_graph.G()); node != lemon::INVALID; ++node) {
		if (lemon::countInArcs(m_graph.G(), node) == 0)
			m_timing_point_calculator.push(node);
	}

	while (!m_timing_point_calculator.empty()) {
		m_timing_point_calculator.process_queue(m_arcs_calculators);
		m_timing_arc_calculator.update(m_timing_point_calculator);
	}

}

} /* namespace timing */
} /* namespace openeda */
