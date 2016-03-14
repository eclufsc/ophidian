/*
 * elmore.h
 *
 *  Created on: Mar 11, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_ELMORE_H_
#define SRC_TIMING_ELMORE_H_

#include "rc_tree.h"
#include <lemon/maps.h>
namespace openeda {
namespace timing {

class elmore {
	rc_tree& m_tree;
	lemon::ListGraph::NodeMap< quantity< si::time > > m_elmore_delay;
	lemon::ListGraph::NodeMap< quantity< si::capacitance > > m_downstream_capacitance;
	lemon::ListGraph::NodeMap< std::pair<rc_tree::capacitor_id, rc_tree::resistor_id> > m_pred;
	std::vector< rc_tree::capacitor_id > m_order;
	rc_tree::capacitor_id m_source;
public:
	elmore(rc_tree& tree, rc_tree::capacitor_id source);
	virtual ~elmore();

	void update();

	quantity<si::time> get(rc_tree::capacitor_id capacitor) const {
		return m_elmore_delay[capacitor];
	}
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_ELMORE_H_ */
