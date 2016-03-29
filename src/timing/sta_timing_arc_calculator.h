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
	const library & m_library;

public:
	sta_timing_arc_calculator(const library & lib);
	virtual ~sta_timing_arc_calculator();

	void update(const graph& g, const graph_nodes_timing& nodes_timing, sta_timing_point_calculator & tpoints, graph_arcs_timing & m_arcs) {
		// PARALEL >>>>
		for (lemon::ListDigraph::Arc arc : m_to_process) {
			const boost::units::quantity<boost::units::si::capacitance> target_load(nodes_timing.load(g.edge_target(arc)));
			const boost::units::quantity<boost::units::si::time> source_slew(nodes_timing.slew(g.edge_source(arc)));
			const entity::entity tarc = g.edge_entity(arc);

			std::cout << "tarc " << tarc.id() << " slew " << source_slew << " load " << target_load << std::endl;

			switch (m_arcs.transition(arc)) {
			case edges::RISE:
				m_arcs.delay(arc, m_library.timing_arc_rise_delay(tarc).compute(target_load, source_slew));
				m_arcs.slew(arc, m_library.timing_arc_rise_slew(tarc).compute(target_load, source_slew));
			case edges::FALL:
				m_arcs.slew(arc, m_library.timing_arc_fall_slew(tarc).compute(target_load, source_slew));
				m_arcs.delay(arc, m_library.timing_arc_fall_delay(tarc).compute(target_load, source_slew));
			default:
				break;
			}
		}
		// <<<<
		// CRITICAL >>>>
		for (lemon::ListDigraph::Arc arc : m_to_process) {
			tpoints.push(g.edge_target(arc));
		}
		// <<<<
		m_to_process.clear();
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_TIMING_ARC_CALCULATOR_H_ */
