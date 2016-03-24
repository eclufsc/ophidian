/*
 * sta_flute_net_calculator.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_
#define SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "../timing/graph_arcs_timing.h"
#include "../timing/graph_nodes_timing.h"
#include <unordered_set>

#include "../timing/elmore.h"
#include "../timing/sta_arc_calculator.h"
#include "flute_rc_tree_estimation.h"

#include "../timing/sta_arc_calculator.h"
#include "../timing/graph.h"

namespace openeda {
namespace timingdriven_placement {

class sta_flute_net_calculator : public timing::sta_arc_calculator, public timing::sta_net_calculator {
	const timing::graph & m_graph;


	const placement::placement & m_placement; // to get pin positions

	// dirty nets
	std::unordered_set<entity::entity> m_dirty;


	entity::vector_property<std::pair<std::size_t, std::size_t> > m_net_rise_edges;
	entity::vector_property<std::pair<std::size_t, std::size_t> > m_net_fall_edges;
	std::vector<lemon::ListDigraph::Arc> m_net2arc;

	void set_delays(timing::graph_nodes_timing& m_nodes, timing::graph_arcs_timing& m_arcs, lemon::ListDigraph::Node source_rise_node, const interconnection::rc_tree& tree, const timing::elmore& delay, const std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> & tap_map,
			const std::size_t first_index, const std::size_t last_index);
public:
	sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, netlist::netlist & netlist);
	virtual ~sta_flute_net_calculator();

	template<class T>
	void net_rise_edges(entity::entity net, T begin, T end) {
		std::size_t net_index(m_placement.netlist().net_system().lookup(net));
		std::size_t begin_index = m_net2arc.size();
		std::size_t size = std::distance(begin, end);
		m_net_rise_edges[net_index].first = begin_index;
		m_net_rise_edges[net_index].first = size;
		m_net2arc.insert(begin, end);
	}

	template<class T>
	void net_fall_edges(entity::entity net, T begin, T end) {
		std::size_t net_index(m_placement.netlist().net_system().lookup(net));
		std::size_t begin_index = m_net2arc.size();
		std::size_t size = std::distance(begin, end);
		m_net_fall_edges[net_index].first = begin_index;
		m_net_fall_edges[net_index].first = size;
		m_net2arc.insert(begin, end);
	}

	void make_dirty(entity::entity net);

	void update_dirty_nets(const timing::library& library, timing::graph_nodes_timing& nodes,
			timing::graph_arcs_timing& arcs);
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_ */
