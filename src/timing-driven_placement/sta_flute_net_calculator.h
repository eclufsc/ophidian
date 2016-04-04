/*
 * sta_flute_net_calculator.h
 *
 *  Created on: Mar 23, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_
#define SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "../timing/graph_arcs_timing.h"
#include "../timing/graph_nodes_timing.h"
#include <unordered_set>

#include "../timing/elmore.h"

#include "../timing/elmore_second_moment.h"
#include "../timing/sta_arc_calculator.h"
#include "flute_rc_tree_estimation.h"

#include "../timing/sta_arc_calculator.h"
#include "../timing/graph.h"
#include "../timing/design_constraints.h"

namespace openeda {
namespace timingdriven_placement {

class sta_flute_net_calculator: public timing::sta_interconnection_estimator {
	const timing::graph & m_graph;
	const placement::placement & m_placement; // to get pin positions
    const timing::library & m_timing_lib; // to get pin capacitances
    flute_rc_tree_creator m_flute;

    entity::vector_property< entity::entity > m_net_sources;

public:
    sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, const timing::library & timing_lib,  netlist::netlist & netlist);
	virtual ~sta_flute_net_calculator();
    void update_net(timing::sta_timing_net_edge_calculator * tnet, entity::entity net, timing::graph_nodes_timing &nodes_timing);
};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_ */
