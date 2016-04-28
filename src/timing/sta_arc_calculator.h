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

#ifndef SRC_TIMING_STA_ARC_CALCULATOR_H_
#define SRC_TIMING_STA_ARC_CALCULATOR_H_

#include <lemon/list_graph.h>
#include "graph.h"
#include "sta_timing_point_calculator.h"
#include "library.h"
#include "graph_arcs_timing.h"
#include "graph_nodes_timing.h"
#include <unordered_set>

namespace ophidian {
namespace timing {

class sta_timing_point_calculator;
class sta_timing_edge_calculator {
protected:
public:
    virtual ~sta_timing_edge_calculator() {}
    virtual void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs) = 0;
};


class sta_timing_arc_edge_calculator : public sta_timing_edge_calculator {
    const library & m_lib;
public:
    sta_timing_arc_edge_calculator(const library & lib);
    void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs);
};


class sta_timing_net_edge_calculator : public sta_timing_edge_calculator {
    lemon::ListDigraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_delay;
    lemon::ListDigraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_slew;
public:
    sta_timing_net_edge_calculator(const graph & g);
    void elmore_delay(lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time> delay);
    void elmore_slew( lemon::ListDigraph::Node node, boost::units::quantity<boost::units::si::time > slew);
    void update(const graph & g, const graph::edge arc, const graph_nodes_timing & nodes, graph_arcs_timing & arcs);
};

class sta_interconnection_estimator {
public:
    virtual void update_net(timing::sta_timing_net_edge_calculator * tnet, entity::entity net, timing::graph_nodes_timing &nodes_timing) = 0;
};



} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_STA_ARC_CALCULATOR_H_ */
