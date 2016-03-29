/*
 * graph.cpp
 *
 *  Created on: Mar 24, 2016
 *      Author: csguth
 */

#include "graph.h"

namespace openeda {
namespace timing {

graph::graph() :
		m_pins(m_graph),
		m_tarcs(m_graph) {
}

graph::~graph() {
}

graph::node graph::node_create(entity::entity pin, std::unordered_map<entity::entity, node>& map) {
	auto result = map.find(pin);
	if (result != map.end())
		return result->second;
	auto new_node = m_graph.addNode();
	map[pin] = new_node;
	m_pins[new_node] = pin;
	return new_node;
}

graph::node graph::rise_node_create(entity::entity pin) {
	return node_create(pin, m_rise_nodes);
}
graph::node graph::fall_node_create(entity::entity pin) {
	return node_create(pin, m_fall_nodes);
}

graph::edge graph::edge_create(node u, node v, entity::entity tarc) {
	auto edge = m_graph.addArc(u, v);
	m_tarcs[edge] = tarc;
	return edge;
}

}
/* namespace timing */
} /* namespace openeda */

