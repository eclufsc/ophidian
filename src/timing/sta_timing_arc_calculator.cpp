/*
 * sta_timing_arc_calculator.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "sta_timing_arc_calculator.h"

namespace openeda {
namespace timing {

sta_timing_arc_calculator::sta_timing_arc_calculator(const graph& graph, const library & lib, const graph_nodes_timing& nodes_timing, graph_arcs_timing & arcs) :
		m_graph(graph), m_library(lib), m_nodes_timing(nodes_timing), m_arcs(arcs) {
}

sta_timing_arc_calculator::~sta_timing_arc_calculator() {
}

} /* namespace timing */
} /* namespace openeda */
