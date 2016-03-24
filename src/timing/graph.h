/*
 * graph.h
 *
 *  Created on: Mar 24, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_GRAPH_H_
#define SRC_TIMING_GRAPH_H_

#include <lemon/list_graph.h>
#include "../entity/entity.h"

namespace openeda {
namespace timing {

class graph {
public:
	using graph_t = lemon::ListDigraph;
	using node = graph_t::Node;
	using edge = graph_t::Arc;
private:
	graph_t m_graph;

	std::unordered_map<entity::entity, node> m_rise_nodes;
	std::unordered_map<entity::entity, node> m_fall_nodes;

	lemon::ListDigraph::NodeMap< entity::entity > m_pins;
	lemon::ListDigraph::ArcMap< entity::entity > m_tarcs;

	node node_create(entity::entity pin, std::unordered_map<entity::entity, node>& map);


public:
	graph();
	virtual ~graph();

	const graph_t & G() const {
		return m_graph;
	}

	std::size_t nodes_count() const {
		return lemon::countNodes(m_graph);
	}

	std::size_t edges_count() const {
		return lemon::countArcs(m_graph);
	}

	node rise_node_create(entity::entity pin);
	node rise_node(entity::entity pin) const {
		return m_rise_nodes.at(pin);
	}

	node fall_node_create(entity::entity pin);
	node fall_node(entity::entity pin) const {
		return m_fall_nodes.at(pin);
	}

	edge edge_create(node u, node v, entity::entity tarc = entity::entity{});
	entity::entity edge_timing_arc(edge e) const{
		return m_tarcs[e];
	}

	entity::entity pin(node u) const {
		return m_pins[u];
	}

	graph_t::OutArcIt out_edges_it(node u) const {
		return graph_t::OutArcIt(m_graph, u);
	}

	node edge_source(edge e)  const {
		return m_graph.source(e);
	}
	node edge_target(edge e)  const {
		return m_graph.target(e);
	}

};

}
/* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_GRAPH_H_ */
