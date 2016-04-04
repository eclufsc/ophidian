/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef SRC_TIMING_GRAPH_NODES_TIMING_H_
#define SRC_TIMING_GRAPH_NODES_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "../entity/entity.h"

namespace ophidian {
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
