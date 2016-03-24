/*
 * graph_nodes_timing.h
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_GRAPH_NODES_TIMING_H_
#define SRC_TIMING_GRAPH_NODES_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "../entity/entity.h"

namespace openeda {
namespace timing {

class graph_nodes_timing {
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_arrivals;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_slews;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::time> > m_requireds;
	lemon::ListDigraph::NodeMap<boost::units::quantity<boost::units::si::capacitance> > m_loads;
public:
	graph_nodes_timing(const lemon::ListDigraph & graph);
	virtual ~graph_nodes_timing();

	void arrival(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> arrival);
	void slew(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> slew);
	void required(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> required);
	void load(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::capacitance> load);

	boost::units::quantity<boost::units::si::time> arrival(lemon::ListDigraph::Node node) const {
		return m_arrivals[node];
	}
	boost::units::quantity<boost::units::si::time> slew(lemon::ListDigraph::Node node) const {
		return m_slews[node];
	}
	boost::units::quantity<boost::units::si::time> required(lemon::ListDigraph::Node node) const {
		return m_requireds[node];
	}
	boost::units::quantity<boost::units::si::capacitance> load(lemon::ListDigraph::Node node) const {
		return m_loads[node];
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_GRAPH_NODES_TIMING_H_ */
