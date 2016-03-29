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
#include "design_constraints.h"
#include "../netlist/netlist.h"

namespace openeda {
namespace timing {

class static_timing_analysis {
	const graph & m_graph;
	const library & m_library;

	graph_nodes_timing m_nodes;
	graph_arcs_timing m_arcs;
	sta_net_calculator * m_interconnection_estimator;

	sta_arc_calculator m_net_arc_calculator;
	sta_timing_arc_calculator m_timing_arc_calculator;
	sta_timing_point_calculator m_timing_point_calculator;

	lemon::ListDigraph::ArcMap<sta_arc_calculator *> m_arcs_calculators;

public:
	static_timing_analysis(const graph & graph, const library & lib, sta_net_calculator * net_calculator);
	virtual ~static_timing_analysis();

	void make_dirty(entity::entity net);

	void set_constraints(const netlist::netlist & netlist, const design_constraints & dc);

	void run();

	boost::units::quantity<boost::units::si::time> rise_arrival(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.rise_node(pin)) == edges::RISE);
		return m_nodes.arrival(m_graph.rise_node(pin));
	}

	boost::units::quantity<boost::units::si::time> fall_arrival(entity::entity pin) const {
        assert(m_graph.node_edge(m_graph.fall_node(pin)) == edges::FALL);
		return m_nodes.arrival(m_graph.fall_node(pin));
	}

	boost::units::quantity<boost::units::si::time> rise_slew(entity::entity pin) const {
		return m_nodes.slew(m_graph.rise_node(pin));
	}

	boost::units::quantity<boost::units::si::time> fall_slew(entity::entity pin) const {
		return m_nodes.slew(m_graph.fall_node(pin));
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STATIC_TIMING_ANALYSIS_H_ */
