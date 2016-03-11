/*
 * rc_tree.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: csguth
 */

#include "rc_tree.h"

namespace openeda {
namespace timing {

rc_tree::rc_tree() :
		m_names(m_graph), m_capacitances(m_graph), m_resistances(m_graph), m_lumped_capacitance(
				0.0 * si::farad) {

}

rc_tree::~rc_tree() {
}

lemon::ListGraph::Node rc_tree::capacitor_insert(std::string name) {
	auto result = m_name2node.find(name);
	if (result != m_name2node.end())
		return result->second;
	auto node = m_graph.addNode();
	m_name2node[name] = node;
	m_capacitances[node] = 0.0*si::farad;
	return node;
}

lemon::ListGraph::Edge rc_tree::resistor_insert(lemon::ListGraph::Node u,
		lemon::ListGraph::Node v, quantity<si::resistance> res) {
	auto resistor = m_graph.addEdge(u, v);
	m_resistances[resistor] = res;
	return resistor;
}

void rc_tree::capacitance(lemon::ListGraph::Node u,
		quantity<si::capacitance> cap) {
	m_lumped_capacitance -= m_capacitances[u];
	m_capacitances[u] = cap;
	m_lumped_capacitance += m_capacitances[u];
}

} /* namespace timing */
} /* namespace openeda */

