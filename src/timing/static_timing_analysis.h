/*
 * static_timing_analysis.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_STATIC_TIMING_ANALYSIS_H_
#define SRC_TIMING_STATIC_TIMING_ANALYSIS_H_

#include "sta_arc_calculator.h"
#include "sta_timing_arc_calculator.h"
#include "sta_timing_point_calculator.h"
#include "graph.h"

namespace openeda {
namespace timing {

class static_timing_analysis {
	const timing::graph & m_graph;

	graph_nodes_timing m_nodes;
	graph_arcs_timing m_arcs;

	sta_net_calculator * m_net_calculator;
	sta_timing_arc_calculator m_timing_arc_calculator;
	sta_timing_point_calculator m_timing_point_calculator;

	lemon::ListDigraph::ArcMap< sta_arc_calculator * > m_arcs_calculators;

public:
	static_timing_analysis(const timing::graph & graph, const library & lib);
	virtual ~static_timing_analysis();
	void net_calculator(sta_net_calculator * net_calculator);
	void arc_calculator(lemon::ListDigraph::Arc arc, sta_arc_calculator * arc_calculator);

	void make_dirty(entity::entity net);
	void run();
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STATIC_TIMING_ANALYSIS_H_ */
