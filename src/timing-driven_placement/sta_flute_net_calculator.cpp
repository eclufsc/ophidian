/*
 * sta_flute_net_calculator.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#include "sta_flute_net_calculator.h"

namespace openeda {
namespace timingdriven_placement {

sta_flute_net_calculator::sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, netlist::netlist & netlist) :
		m_graph(graph), m_placement(placement) {
	netlist.register_net_property(&m_net_rise_edges);
	netlist.register_net_property(&m_net_fall_edges);
}

sta_flute_net_calculator::~sta_flute_net_calculator() {
}

void sta_flute_net_calculator::make_dirty(entity::entity net) {
	m_dirty.insert(net);
}

void sta_flute_net_calculator::set_delays(timing::graph_nodes_timing& m_nodes, timing::graph_arcs_timing& m_arcs, lemon::ListDigraph::Node source_node, const interconnection::rc_tree& tree, const timing::elmore& delay,
		const std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> & tap_map, const std::size_t first_index, const std::size_t last_index) {
	m_nodes.load(source_node, tree.lumped());
	for (std::size_t i { first_index }; i < last_index; ++i) {
		lemon::ListDigraph::Node target_node { m_graph.edge_target(m_net2arc[i]) };
		entity::entity target_pin { m_graph.pin(target_node) };
		m_arcs.delay(m_net2arc[i], delay.get(tap_map.at(target_pin)));
	}
}

void sta_flute_net_calculator::update_dirty_nets(const timing::library& m_library, timing::graph_nodes_timing& m_nodes, timing::graph_arcs_timing& m_arcs) {
	for (auto net : m_dirty) {

		interconnection::rc_tree tree;
		auto tap_map = flute_rc_tree(m_placement, net, tree, m_library);
		std::size_t net_index = m_placement.netlist().net_system().lookup(net);

		std::pair<std::size_t, std::size_t> rise_edges { m_net_rise_edges[net_index] };
		lemon::ListDigraph::Node source_rise_node { m_graph.edge_source(m_net2arc[rise_edges.first]) };
		std::pair<std::size_t, std::size_t> fall_edges { m_net_rise_edges[net_index] };
		lemon::ListDigraph::Node source_fall_node { m_graph.edge_source(m_net2arc[fall_edges.first]) };

		entity::entity source_pin { m_graph.pin(source_rise_node) };
		timing::elmore delay(tree, tap_map.at(source_pin));

		set_delays(m_nodes, m_arcs, source_rise_node, tree, delay, tap_map, rise_edges.first, rise_edges.first + rise_edges.second);
		set_delays(m_nodes, m_arcs, source_fall_node, tree, delay, tap_map, fall_edges.first, fall_edges.first + fall_edges.second);
	}
	m_dirty.clear();
}

} /* namespace timing */
} /* namespace openeda */
