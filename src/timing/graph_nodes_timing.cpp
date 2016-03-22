/*
 * graph_nodes_timing.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: csguth
 */

#include "graph_nodes_timing.h"

namespace openeda {
namespace timing {

graph_nodes_timing::graph_nodes_timing(lemon::ListDigraph & graph) :
	m_arrivals(graph),
	m_slews(graph),
	m_requireds(graph),
	m_loads(graph)
{

}

graph_nodes_timing::~graph_nodes_timing() {
	// TODO Auto-generated destructor stub
}

void graph_nodes_timing::arrival(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> arrival) {
	m_arrivals[node] = arrival;
}

void graph_nodes_timing::slew(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> slew) {
	m_slews[node] = slew;
}

void graph_nodes_timing::required(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> required) {
	m_requireds[node] = required;
}

void graph_nodes_timing::load(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::capacitance> load) {
	m_loads[node] = load;
}

} /* namespace timing */
} /* namespace openeda */

