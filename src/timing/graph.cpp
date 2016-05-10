/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "graph.h"

namespace ophidian {
namespace timing {

graph::graph() :
		m_pins(m_graph),
		m_node_edges(m_graph),
		m_arc_types(m_graph),
		m_arcs(m_graph){
}

graph::~graph() {
}

void graph::test_insert(node ck, node d, entity_system::entity tarc)
{
    m_tests.push_back(test{ck, d, tarc});
}

graph::node graph::node_create(entity_system::entity pin, edges node_edge, std::unordered_map< entity_system::entity, node, entity_system::entity_hash > & map) {
	auto new_node = m_graph.addNode();
	if(map.find(pin) == map.end())
		map[pin] = new_node;
	m_pins[new_node] = pin;
	m_node_edges[new_node] = node_edge;
	return new_node;
}

graph::node graph::rise_node_create(entity_system::entity pin) {
	return node_create(pin, edges::RISE, m_rise_nodes);
}
graph::node graph::fall_node_create(entity_system::entity pin) {
	return node_create(pin, edges::FALL, m_fall_nodes);
}

void graph::node_edge(node u, edges e) {
	m_node_edges[u] = e;
}

graph::edge graph::edge_create(node u, node v, edge_types type, entity_system::entity tarc) {
	auto edge = m_graph.addArc(u, v);
	m_arcs[edge] = tarc;
	m_arc_types[edge] = type;
	return edge;
}

void graph::edge_source(edge e, node u) {
	m_graph.changeSource(e, u);
}

}
/* namespace timing */
} /* namespace ophidian */

