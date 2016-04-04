/*
 * sta_timing_point_calculator.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_
#define TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_

#include "sta_arc_calculator.h"
#include "graph.h"

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"

namespace ophidian {
namespace timing {

class sta_timing_edge_calculator;
class sta_timing_point_calculator {
    const graph & m_graph;
	std::deque<lemon::ListDigraph::Node> m_to_process;
    lemon::ListDigraph::NodeMap<std::size_t> m_counter;
public:
    sta_timing_point_calculator(const graph & g);
	virtual ~sta_timing_point_calculator();

    void reset_counter();

	bool empty();
	virtual void push(lemon::ListDigraph::Node node);
	virtual void process_queue(const graph & m_graph, const graph_arcs_timing & m_arcs_timing, graph_nodes_timing & m_nodes_timing, lemon::ListDigraph::ArcMap< sta_timing_edge_calculator * > & tarcs);
};
} /* namespace timing */
} /* namespace openeda */

#endif /* TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_ */
