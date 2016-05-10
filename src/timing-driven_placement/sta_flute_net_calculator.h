/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
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

    entity_system::vector_property< entity_system::entity > m_net_sources;

public:
    sta_flute_net_calculator(const timing::graph & graph, const placement::placement & placement, const timing::library & timing_lib,  netlist::netlist & netlist);
	virtual ~sta_flute_net_calculator();
    void update_net(timing::sta_timing_net_edge_calculator * tnet, entity_system::entity net, timing::graph_nodes_timing &nodes_timing);
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_DRIVEN_PLACEMENT_STA_FLUTE_NET_CALCULATOR_H_ */
