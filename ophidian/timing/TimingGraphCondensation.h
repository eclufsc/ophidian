/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H
#define OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H

#include <ophidian/timing/TimingGraph.h>

namespace ophidian
{
namespace timing
{

class GraphNodesTiming
{
public:
    using time_unit_type                      = util::second_t;
    using capacitance_unit_type               = util::farad_t;

    using timing_graph_type                   = TimingGraph;
    using node_type                           = timing_graph_type::node_type;
    using timing_node_to_time_map_type        = timing_graph_type::graph_type::NodeMap<time_unit_type>;
    using timing_node_to_capacitance_map_type = timing_graph_type::graph_type::NodeMap<capacitance_unit_type>;

    GraphNodesTiming(const timing_graph_type & graph);
    virtual ~GraphNodesTiming();

    void arrival(node_type node, time_unit_type arrival);
    void slew(node_type node, time_unit_type slew);
    void required(node_type node, time_unit_type required);
    void load(node_type node, capacitance_unit_type load);

    const time_unit_type arrival(node_type node) const;
    const time_unit_type slew(node_type node) const;
    const time_unit_type required(node_type node) const;
    const capacitance_unit_type load(node_type node) const;

private:
    timing_node_to_time_map_type        mArrivals;
    timing_node_to_time_map_type        mSlews;
    timing_node_to_time_map_type        mRequireds;
    timing_node_to_capacitance_map_type mLoads;
};


class GraphArcsTiming
{
public:

    using time_unit_type              = util::second_t;

    using timing_graph_type           = TimingGraph;
    using arc_type                    = TimingGraph::arc_type;
    using timing_arc_to_time_map_type = timing_graph_type::graph_type::ArcMap<time_unit_type>;

    GraphArcsTiming(const timing_graph_type & graph);
    virtual ~GraphArcsTiming();

    void delay(arc_type arc, const time_unit_type delay);
    void slew(arc_type arc, const time_unit_type delay);

    const time_unit_type delay(arc_type arc) const;
    const time_unit_type slew(arc_type arc) const;

private:
    timing_arc_to_time_map_type mDelays;
    timing_arc_to_time_map_type mSlews;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H
