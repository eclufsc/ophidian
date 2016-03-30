/*
 * sta_net_calculator.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_STA_ARC_CALCULATOR_H_
#define SRC_TIMING_STA_ARC_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "graph.h"
#include "sta_timing_point_calculator.h"
#include "library.h"
#include "graph_arcs_timing.h"
#include "graph_nodes_timing.h"

namespace openeda {
namespace timing {

class sta_timing_point_calculator;
class sta_arc_calculator {
protected:
	std::deque<lemon::ListDigraph::Arc> m_to_process;
public:
	virtual ~sta_arc_calculator() {
	}
	virtual void push(lemon::ListDigraph::Arc arc) {
        assert(std::find( m_to_process.begin(), m_to_process.end(), arc) == m_to_process.end());
		m_to_process.push_back(arc);
	}
	virtual void update(const graph& m_graph, const graph_nodes_timing& nodes_timing, sta_timing_point_calculator & tpoints, graph_arcs_timing & m_arcs);
};

class sta_net_calculator {
public:
	virtual ~sta_net_calculator() {
	}
	virtual void make_dirty(entity::entity net) = 0;
	virtual void update_dirty_nets(const library& library, graph_nodes_timing& nodes, graph_arcs_timing& arcs) = 0;
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_STA_ARC_CALCULATOR_H_ */
