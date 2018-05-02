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
    GraphNodesTiming::GraphNodesTiming(const TimingGraph & graph) :
        mArrivals(graph.graph()),
        mSlews(graph.graph()),
        mRequireds(graph.graph()),
        mLoads(graph.graph())
    {
    }

    GraphNodesTiming::~GraphNodesTiming()
    {
    }

    void GraphNodesTiming::arrival(NodeType node, util::second_t arrival)
    {
        mArrivals[node] = arrival;
    }

    void GraphNodesTiming::slew(NodeType node, util::second_t slew)
    {
        mSlews[node] = slew;
    }

    void GraphNodesTiming::required(NodeType node, util::second_t required)
    {
        mRequireds[node] = required;
    }

    void GraphNodesTiming::load(NodeType node, util::farad_t load)
    {
        mLoads[node] = load;
    }

    const util::second_t GraphNodesTiming::arrival(NodeType node) const
    {
        return mArrivals[node];
    }

    const util::second_t GraphNodesTiming::slew(NodeType node) const
    {
        return mSlews[node];
    }

    const util::second_t GraphNodesTiming::required(NodeType node) const
    {
        return mRequireds[node];
    }

    const util::farad_t GraphNodesTiming::load(NodeType node) const
    {
        return mLoads[node];
    }

// ------------------------------------------------------------------------------

    GraphArcsTiming::GraphArcsTiming(const TimingGraph & graph) :
        mDelays(graph.graph()),
        mSlews(graph.graph())
    {
    }

    GraphArcsTiming::~GraphArcsTiming()
    {
    }

    void GraphArcsTiming::delay(ArcType arc, const util::second_t delay)
    {
        mDelays[arc] = delay;
    }

    void GraphArcsTiming::slew(ArcType arc, const util::second_t slew)
    {
        mSlews[arc] = slew;
    }

    const util::second_t GraphArcsTiming::delay(ArcType arc) const
    {
        return mDelays[arc];
    }
    const util::second_t GraphArcsTiming::slew(ArcType arc) const
    {
        return mSlews[arc];
    }

}   // namespace timing
}   // namespace ophidian
