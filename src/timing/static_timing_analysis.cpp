/*
 * static_timing_analysis.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#include "static_timing_analysis.h"

namespace openeda {
namespace timing {

static_timing_analysis::static_timing_analysis(const graph & graph, const library & lib, sta_net_calculator * net_calculator) :
				m_graph(graph), m_library(lib), m_nodes(graph.G()), m_arcs(graph.G()), m_interconnection_estimator(net_calculator), m_timing_arc_calculator(lib), m_arcs_calculators(m_graph.G()) {

	for (lemon::ListDigraph::ArcIt it(graph.G()); it != lemon::INVALID; ++it) {
		timing::graph::node source { graph.edge_source(static_cast<timing::graph::edge>(it)) };
		timing::graph::node target { graph.edge_target(static_cast<timing::graph::edge>(it)) };
		switch (graph.edge_type(static_cast<timing::graph::edge>(it))) {
		case timing::edge_types::NET:
			m_arcs_calculators[it] = &m_net_arc_calculator;
			break;
		case timing::edge_types::TIMING_ARC:
			m_arcs_calculators[it] = &m_timing_arc_calculator;
			break;
		}
	}
}

static_timing_analysis::~static_timing_analysis() {
}

void static_timing_analysis::make_dirty(entity::entity net) {
	m_interconnection_estimator->make_dirty(net);
}

void static_timing_analysis::run() {
	m_interconnection_estimator->update_dirty_nets(m_library, m_nodes, m_arcs);


	for (lemon::ListDigraph::NodeIt node(m_graph.G()); node != lemon::INVALID; ++node) {
		if (lemon::countInArcs(m_graph.G(), node) == 0)
			m_timing_point_calculator.push(node);
	}

	while (!m_timing_point_calculator.empty()) {
		m_timing_point_calculator.process_queue(m_graph, m_arcs, m_nodes, m_arcs_calculators);
		m_timing_arc_calculator.update(m_graph, m_nodes, m_timing_point_calculator, m_arcs);
		m_net_arc_calculator.update(m_graph, m_nodes, m_timing_point_calculator, m_arcs);
	}

}

void static_timing_analysis::set_constraints(const netlist::netlist & netlist, const design_constraints & dc) {

	using namespace boost::units;
	using namespace boost::units::si;
	for(auto & i : dc.input_delays)
	{
		auto pin = netlist.pin_by_name(i.port_name);
		m_nodes.arrival( m_graph.rise_node(pin), quantity<si::time>(i.delay*pico*seconds) );
		m_nodes.arrival( m_graph.fall_node(pin), quantity<si::time>(i.delay*pico*seconds) );
	}

	for(auto & i : dc.input_drivers)
	{
		auto pin = netlist.pin_by_name(i.port_name);
		m_nodes.slew( m_graph.rise_node(pin), quantity<si::time>(i.slew_rise*pico*seconds) );
		m_nodes.slew( m_graph.fall_node(pin), quantity<si::time>(i.slew_fall*pico*seconds) );
	}

}

} /* namespace timing */
} /* namespace openeda */

