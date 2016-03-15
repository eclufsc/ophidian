/*
 * elmore.h
 *
 *  Created on: Mar 11, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_ELMORE_H_
#define SRC_TIMING_ELMORE_H_

#include "../interconnection/rc_tree.h"
#include <lemon/maps.h>
namespace openeda {
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

	boost::units::quantity<boost::units::si::time> get(interconnection::rc_tree::capacitor_id capacitor) const {
		return m_elmore_delay[capacitor];
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_ELMORE_H_ */
