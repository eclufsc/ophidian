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

#include "TimingGraphCondensation.h"

namespace ophidian
{
namespace timing
{
    GraphNodesTiming::GraphNodesTiming(const timing_graph_type & graph) :
        mArrivals(graph.graph()),
        mSlews(graph.graph()),
        mRequireds(graph.graph()),
        mLoads(graph.graph())
    {
    }

    GraphNodesTiming::~GraphNodesTiming()
    {
    }

    void GraphNodesTiming::arrival(node_type node, time_unit_type arrival)
    {
        mArrivals[node] = arrival;
    }

    void GraphNodesTiming::slew(node_type node, time_unit_type slew)
    {
        mSlews[node] = slew;
    }

    void GraphNodesTiming::required(node_type node, time_unit_type required)
    {
        mRequireds[node] = required;
    }

    void GraphNodesTiming::load(node_type node, capacitance_unit_type load)
    {
        mLoads[node] = load;
    }

    const GraphNodesTiming::time_unit_type GraphNodesTiming::arrival(node_type node) const
    {
        return mArrivals[node];
    }

    const GraphNodesTiming::time_unit_type GraphNodesTiming::slew(node_type node) const
    {
        return mSlews[node];
    }

    const GraphNodesTiming::time_unit_type GraphNodesTiming::required(node_type node) const
    {
        return mRequireds[node];
    }

    const GraphNodesTiming::capacitance_unit_type GraphNodesTiming::load(node_type node) const
    {
        return mLoads[node];
    }

// ------------------------------------------------------------------------------

    GraphArcsTiming::GraphArcsTiming(const timing_graph_type & graph) :
        mDelays(graph.graph()),
        mSlews(graph.graph())
    {
    }

    GraphArcsTiming::~GraphArcsTiming()
    {
    }

    void GraphArcsTiming::delay(arc_type arc, const time_unit_type delay)
    {
        mDelays[arc] = delay;
    }

    void GraphArcsTiming::slew(arc_type arc, const time_unit_type slew)
    {
        mSlews[arc] = slew;
    }

    const GraphArcsTiming::time_unit_type GraphArcsTiming::delay(arc_type arc) const
    {
        return mDelays[arc];
    }
    const GraphArcsTiming::time_unit_type GraphArcsTiming::slew(arc_type arc) const
    {
        return mSlews[arc];
    }

}   // namespace timing
}   // namespace ophidian
