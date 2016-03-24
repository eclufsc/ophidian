/*
 * arcs_timing.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "graph_arcs_timing.h"

namespace openeda {
namespace timing {

using namespace boost::units;

graph_arcs_timing::graph_arcs_timing(const lemon::ListDigraph & graph) :
		m_transitions(graph), m_delays(graph), m_slews(graph) {
}

graph_arcs_timing::~graph_arcs_timing() {
}

void graph_arcs_timing::delay(lemon::ListDigraph::Arc arc, const quantity<si::time> delay) {
	m_delays[arc] = delay;
}

void graph_arcs_timing::slew(lemon::ListDigraph::Arc arc, const quantity<si::time> delay) {
	m_slews[arc] = delay;
}

void graph_arcs_timing::transition(lemon::ListDigraph::Arc arc, edge trans) {
	m_transitions[arc] = trans;
}

} /* namespace timing */
} /* namespace openeda */

