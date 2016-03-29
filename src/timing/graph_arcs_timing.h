/*
 * arcs_timing.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_GRAPH_ARCS_TIMING_H_
#define SRC_TIMING_GRAPH_ARCS_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "transition.h"
namespace openeda {
namespace timing {

class graph_arcs_timing {
	lemon::ListDigraph::ArcMap<edges> m_transitions;
	lemon::ListDigraph::ArcMap<boost::units::quantity<boost::units::si::time> > m_delays;
	lemon::ListDigraph::ArcMap<boost::units::quantity<boost::units::si::time> > m_slews;
public:
	graph_arcs_timing(const lemon::ListDigraph & graph);
	virtual ~graph_arcs_timing();
	void delay(lemon::ListDigraph::Arc, const boost::units::quantity<boost::units::si::time> delay);
	const boost::units::quantity<boost::units::si::time> delay(lemon::ListDigraph::Arc arc) const {
		return m_delays[arc];
	}
	void slew(lemon::ListDigraph::Arc, const boost::units::quantity<boost::units::si::time> delay);
	const boost::units::quantity<boost::units::si::time> slew(lemon::ListDigraph::Arc arc) const {
		return m_slews[arc];
	}

	void transition(lemon::ListDigraph::Arc arc, edges trans);
	const edges transition(lemon::ListDigraph::Arc arc) const {
		return m_transitions[arc];
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_GRAPH_ARCS_TIMING_H_ */
