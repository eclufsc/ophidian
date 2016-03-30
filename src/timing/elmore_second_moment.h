/*
 * elmore_second_moment.h
 *
 *  Created on: Mar 28, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_ELMORE_SECOND_MOMENT_H_
#define SRC_TIMING_ELMORE_SECOND_MOMENT_H_

#include "elmore.h"

namespace openeda {
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
