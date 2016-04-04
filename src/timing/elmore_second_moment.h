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

#ifndef SRC_TIMING_ELMORE_SECOND_MOMENT_H_
#define SRC_TIMING_ELMORE_SECOND_MOMENT_H_

#include "elmore.h"

namespace ophidian {
namespace timing {

typedef boost::units::derived_dimension<boost::units::time_base_dimension,2>::type  squared_time_dimension;
typedef boost::units::unit<squared_time_dimension, boost::units::si::system> second_moment;


class elmore_second_moment {
	const elmore & m_elmore;
	const interconnection::rc_tree & m_tree;
	lemon::ListGraph::NodeMap<boost::units::quantity<second_moment> > m_second_moment;
	void update();

public:
	elmore_second_moment(const interconnection::rc_tree & tree, const elmore & e);
	virtual ~elmore_second_moment();
	boost::units::quantity<second_moment> at(interconnection::rc_tree::capacitor_id capacitor) const {
		return m_second_moment[capacitor];
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_ELMORE_SECOND_MOMENT_H_ */
