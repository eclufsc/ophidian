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

#ifndef TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_
#define TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_

#include "sta_arc_calculator.h"
#include "graph.h"

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"

namespace ophidian {
namespace timing {

class sta_timing_edge_calculator;
class sta_timing_point_calculator {
    const graph & m_graph;
	std::deque<lemon::ListDigraph::Node> m_to_process;
    lemon::ListDigraph::NodeMap<std::size_t> m_counter;
public:
    sta_timing_point_calculator(const graph & g);
	virtual ~sta_timing_point_calculator();

    void reset_counter();

	bool empty();
	virtual void push(lemon::ListDigraph::Node node);
	virtual void process_queue(const graph & m_graph, const graph_arcs_timing & m_arcs_timing, graph_nodes_timing & m_nodes_timing, lemon::ListDigraph::ArcMap< sta_timing_edge_calculator * > & tarcs);
};
} /* namespace timing */
} /* namespace openeda */

#endif /* TEST_TIMING_STA_TIMING_POINT_CALCULATOR_H_ */
