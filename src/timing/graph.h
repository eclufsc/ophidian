/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_TIMING_GRAPH_H_
#define SRC_TIMING_GRAPH_H_

#include <lemon/list_graph.h>
#include "../entity/entity.h"
#include "transition.h"

namespace ophidian {
namespace timing {

enum class  edge_types {
	TIMING_ARC, NET
};


class graph {
public:
	using graph_t = lemon::ListDigraph;
	using node = graph_t::Node;
	using edge = graph_t::Arc;
private:
	graph_t m_graph;

	lemon::ListDigraph::NodeMap< entity::entity > m_pins;
	lemon::ListDigraph::NodeMap< edges > m_node_edges;
	lemon::ListDigraph::ArcMap< edge_types > m_arc_types;
	lemon::ListDigraph::ArcMap< entity::entity > m_arcs;


	std::unordered_map< entity::entity, node > m_rise_nodes;
	std::unordered_map< entity::entity, node > m_fall_nodes;

	node node_create(entity::entity pin, edges node_edge, std::unordered_map< entity::entity, node > & map);


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

	void node_edge(node u, edges e);
	edges node_edge(node u) const {
		return m_node_edges[u];
	}


	edge edge_create(node u, node v, edge_types type, entity::entity entity);
	entity::entity edge_entity(edge e) const{
		return m_arcs[e];
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
	edge_types edge_type(edge e) const {
		return m_arc_types[e];
	}

	void edge_source(edge e, node u);


};

}
/* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_GRAPH_H_ */
