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

#ifndef SRC_TIMING_GRAPH_ARCS_TIMING_H_
#define SRC_TIMING_GRAPH_ARCS_TIMING_H_

#include <lemon/list_graph.h>
#include <boost/units/systems/si.hpp>
#include "transition.h"
namespace ophidian {
namespace timing {

class graph_arcs_timing {
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
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_GRAPH_ARCS_TIMING_H_ */
