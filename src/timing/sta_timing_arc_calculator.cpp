/*
 * sta_timing_arc_calculator.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "sta_timing_arc_calculator.h"

namespace openeda {
namespace timing {

sta_timing_arc_calculator::sta_timing_arc_calculator(const library& lib, const lemon::ListDigraph& graph, const graph_nodes_timing& nodes_timing, graph_arcs_timing& arcs_rise, graph_arcs_timing& arcs_fall) :
		m_library(lib), m_graph(graph), m_nodes_timing(nodes_timing), m_arcs_rise(arcs_rise), m_arcs_fall(arcs_fall), m_tarcs(graph) {
}

sta_timing_arc_calculator::~sta_timing_arc_calculator() {
}

void sta_timing_arc_calculator::timing_arc(lemon::ListDigraph::Arc arc, entity::entity timing_arc) {
	m_tarcs[arc] = timing_arc;
}

} /* namespace timing */
} /* namespace openeda */
