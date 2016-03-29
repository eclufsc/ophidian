/*
 * sta_timing_arc_calculator.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_STA_TIMING_ARC_CALCULATOR_H_
#define SRC_TIMING_STA_TIMING_ARC_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"
#include "library.h"

#include "sta_arc_calculator.h"
#include "graph.h"

namespace openeda {
namespace timing {

class sta_timing_arc_calculator: public sta_arc_calculator {
	const graph & m_graph;
	const library & m_library;

	const graph_nodes_timing & m_nodes_timing;
	graph_arcs_timing & m_arcs;

	std::deque<lemon::ListDigraph::Arc> m_to_process;

public:
	sta_timing_arc_calculator(const graph& graph, const library & lib, const graph_nodes_timing& nodes_timing, graph_arcs_timing & arcs);
	virtual ~sta_timing_arc_calculator();

	void push(lemon::ListDigraph::Arc arc) {
		m_to_process.push_back(arc);
	}

	void update(sta_timing_point_calculator & tpoints) {
		// PARALEL >>>>
		for (lemon::ListDigraph::Arc arc : m_to_process) {
			const boost::units::quantity<boost::units::si::capacitance> target_load(m_nodes_timing.load(m_graph.edge_target(arc)));
			const boost::units::quantity<boost::units::si::time> source_slew(m_nodes_timing.slew(m_graph.edge_source(arc)));
			const entity::entity tarc = m_graph.edge_timing_arc(arc);
			switch (m_arcs.transition(arc)) {
			case edge::RISE:
				m_arcs.delay(arc, m_library.timing_arc_rise_delay(tarc).compute(target_load, source_slew));
				m_arcs.slew(arc, m_library.timing_arc_rise_slew(tarc).compute(target_load, source_slew));
			case edge::FALL:
				m_arcs.slew(arc, m_library.timing_arc_fall_slew(tarc).compute(target_load, source_slew));
				m_arcs.delay(arc, m_library.timing_arc_fall_delay(tarc).compute(target_load, source_slew));
			default:
				break;
			}
		}
		// <<<<
		// CRITICAL >>>>
		for (lemon::ListDigraph::Arc arc : m_to_process) {
			tpoints.push(m_graph.edge_target(arc));
		}
		// <<<<
		m_to_process.clear();
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_TIMING_ARC_CALCULATOR_H_ */
