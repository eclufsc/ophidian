/*
 * sta_net_calculator.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_STA_ARC_CALCULATOR_H_
#define SRC_TIMING_STA_ARC_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "../entity/entity.h"
#include "sta_timing_point_calculator.h"

namespace openeda {
namespace timing {

class sta_timing_point_calculator;
class sta_arc_calculator {
public:
	virtual ~sta_arc_calculator() {}
	virtual void push(lemon::ListDigraph::Arc arc){};
	virtual void update(sta_timing_point_calculator & tpoints){};
};



class sta_net_calculator {
public:
	virtual ~sta_net_calculator(){}
	virtual void make_dirty(entity::entity net) = 0;
	virtual void update_dirty_nets(timing::graph_nodes_timing& nodes,
			timing::graph_arcs_timing& arcs) = 0;
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_ARC_CALCULATOR_H_ */
