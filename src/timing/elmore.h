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

#ifndef SRC_TIMING_ELMORE_H_
#define SRC_TIMING_ELMORE_H_

#include "../interconnection/rc_tree.h"
#include <lemon/maps.h>
namespace ophidian {
namespace timing {

class elmore {

	interconnection::rc_tree& m_tree;
	lemon::ListGraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_delay;
	lemon::ListGraph::NodeMap< boost::units::quantity< boost::units::si::capacitance > > m_downstream_capacitance;
	lemon::ListGraph::NodeMap< std::pair<interconnection::rc_tree::capacitor_id, interconnection::rc_tree::resistor_id> > m_pred;
	std::vector< interconnection::rc_tree::capacitor_id > m_order;
	interconnection::rc_tree::capacitor_id m_source;
public:
	elmore(interconnection::rc_tree& tree, interconnection::rc_tree::capacitor_id source);
	virtual ~elmore();

	void update();

	boost::units::quantity<boost::units::si::time> at(interconnection::rc_tree::capacitor_id capacitor) const {
		return m_elmore_delay[capacitor];
	}

	const lemon::ListGraph::NodeMap< std::pair<interconnection::rc_tree::capacitor_id, interconnection::rc_tree::resistor_id> > & pred() const {
		return m_pred;
	}
	const std::vector< interconnection::rc_tree::capacitor_id > & order() const {
		return m_order;
	}
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_ELMORE_H_ */
