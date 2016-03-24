/*
 * sta_timing_point_calculator.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_
#define TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"
#include "sta_arc_calculator.h"
#include "graph.h"

namespace openeda {
namespace timing {

class sta_arc_calculator;
class sta_timing_point_calculator {
	const graph & m_graph;
	const graph_arcs_timing & m_arcs_timing;
	graph_nodes_timing & m_nodes_timing;
	std::deque<lemon::ListDigraph::Node> m_to_process;
public:
	sta_timing_point_calculator(const graph & graph, const graph_arcs_timing & m_arcs_timing, graph_nodes_timing & m_nodes_timing);
	virtual ~sta_timing_point_calculator();

	bool empty();
	virtual void push(lemon::ListDigraph::Node node);
	virtual void process_queue(lemon::ListDigraph::ArcMap< sta_arc_calculator * > & tarcs);
};

} /* namespace timing */
} /* namespace openeda */

#endif /* TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_ */
