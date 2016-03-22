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

namespace openeda {
namespace timing {

class sta_timing_arc_calculator {
	const library & m_library;
	const lemon::ListDigraph & m_graph;
	const graph_nodes_timing& m_nodes_timing;
	graph_arcs_timing & m_arcs_rise;
	graph_arcs_timing & m_arcs_fall;
	lemon::ListDigraph::ArcMap<entity::entity> m_tarcs;

public:
	sta_timing_arc_calculator(const library & lib, const lemon::ListDigraph & graph, const graph_nodes_timing& nodes_timing, graph_arcs_timing & arcs_rise, graph_arcs_timing & arcs_fall);
	virtual ~sta_timing_arc_calculator();

	void timing_arc(lemon::ListDigraph::Arc arc, entity::entity timing_arc);

	template<class T>
	void update(T begin, T end) {
		for (T it(begin); it != end; ++it) {
			lemon::ListDigraph::Arc arc = static_cast<lemon::ListDigraph::Arc>(*it);
			const boost::units::quantity<boost::units::si::capacitance> target_load(m_nodes_timing.load(m_graph.target(arc)));
			const boost::units::quantity<boost::units::si::time> source_slew(m_nodes_timing.slew(m_graph.source(arc)));
			const entity::entity tarc = m_tarcs[arc];
			m_arcs_rise.delay(arc, m_library.timing_arc_rise_delay(tarc).compute(target_load, source_slew));
			m_arcs_fall.delay(arc, m_library.timing_arc_fall_delay(tarc).compute(target_load, source_slew));
			m_arcs_rise.slew(arc, m_library.timing_arc_rise_slew(tarc).compute(target_load, source_slew));
			m_arcs_fall.slew(arc, m_library.timing_arc_fall_slew(tarc).compute(target_load, source_slew));
		}
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_TIMING_ARC_CALCULATOR_H_ */
