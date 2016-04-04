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

namespace ophidian {
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
} /* namespace ophidian */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_ */
