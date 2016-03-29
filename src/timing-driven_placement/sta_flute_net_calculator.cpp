/*
 * sta_flute_net_calculator.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#include "sta_flute_net_calculator.h"
#include "../timing/transition.h"
#include <map>
#include <set>

#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

namespace openeda {
namespace timingdriven_placement {

sta_flute_net_calculator::sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, netlist::netlist & netlist) :
		m_graph(graph), m_placement(placement) {
	netlist.register_net_property(&m_net_rise_edges);
	netlist.register_net_property(&m_net_fall_edges);

	std::map<entity::entity, std::set<lemon::ListDigraph::ArcIt> > nets_rise_arcs;
	std::map<entity::entity, std::set<lemon::ListDigraph::ArcIt> > nets_fall_arcs;

	for (lemon::ListDigraph::ArcIt it(m_graph.G()); it != lemon::INVALID; ++it) {
		lemon::ListDigraph::Node target_node;
		switch (m_graph.edge_type(it)) {
		case timing::edge_types::NET:
			target_node = m_graph.edge_target(it);
			switch (m_graph.node_edge(target_node)) {
			case timing::edges::RISE:
				nets_rise_arcs[m_graph.edge_entity(it)].insert(it);
				break;
			case timing::edges::FALL:
				nets_fall_arcs[m_graph.edge_entity(it)].insert(it);
				break;
			}
			break;
		case timing::edge_types::TIMING_ARC:
		default:
			break;
		}
	}
	for (auto rise_arcs : nets_rise_arcs)
		net_rise_edges(rise_arcs.first, rise_arcs.second.begin(), rise_arcs.second.end());
	for (auto fall_arcs : nets_fall_arcs)
		net_fall_edges(fall_arcs.first, fall_arcs.second.begin(), fall_arcs.second.end());
}

sta_flute_net_calculator::~sta_flute_net_calculator() {
}

void sta_flute_net_calculator::make_dirty(entity::entity net) {
	m_dirty.insert(net);
}

void sta_flute_net_calculator::set_delays(timing::graph_nodes_timing& nodes_timing, timing::graph_arcs_timing& arcs_timing, lemon::ListDigraph::Node source_node, const interconnection::rc_tree& tree, const timing::elmore& delay,
		const timing::elmore_second_moment & second, const std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> & tap_map, const std::size_t first_index, const std::size_t last_index) {
	nodes_timing.load(source_node, tree.lumped());
	for (std::size_t i { first_index }; i <= last_index; ++i) {
		lemon::ListDigraph::Node target_node { m_graph.edge_target(m_net2arc[i]) };
		entity::entity target_pin { m_graph.pin(target_node) };
		auto capacitor = tap_map.at(target_pin);
		auto first_moment = delay.at(capacitor);
		auto second_moment = second.at(capacitor);
		arcs_timing.delay(m_net2arc[i], first_moment);
		arcs_timing.slew(m_net2arc[i],  boost::units::sqrt( second_moment*2.0 - boost::units::pow<2>(first_moment) ));
	}
}

void sta_flute_net_calculator::update_dirty_nets(const timing::library& m_library, timing::graph_nodes_timing& m_nodes, timing::graph_arcs_timing& m_arcs) {
	for (auto net : m_dirty) {
		interconnection::rc_tree tree;
        auto tap_map = m_flute.create_tree(m_placement, net, tree, m_library);
		std::size_t net_index = m_placement.netlist().net_system().lookup(net);
		std::pair<std::size_t, std::size_t> rise_edges { m_net_rise_edges[net_index] };
		lemon::ListDigraph::Node source_rise_node { m_graph.edge_source(m_net2arc[rise_edges.first]) };
        std::pair<std::size_t, std::size_t> fall_edges { m_net_fall_edges[net_index] };
		lemon::ListDigraph::Node source_fall_node { m_graph.edge_source(m_net2arc[fall_edges.first]) };
		entity::entity source_pin { m_graph.pin(source_rise_node) };
		timing::elmore delay(tree, tap_map.at(source_pin));
		timing::elmore_second_moment second(tree, delay);
        set_delays(m_nodes, m_arcs, source_rise_node, tree, delay, second, tap_map, rise_edges.first, rise_edges.first + rise_edges.second -1);
        set_delays(m_nodes, m_arcs, source_fall_node, tree, delay, second, tap_map, fall_edges.first, fall_edges.first + fall_edges.second -1);
	}
	m_dirty.clear();
}

} /* namespace timing */
} /* namespace openeda */
