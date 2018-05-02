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
    using NodeType = TimingGraph::NodeType;

    GraphNodesTiming(const TimingGraph & graph);
    virtual ~GraphNodesTiming();

    void arrival(NodeType node, util::second_t arrival);
    void slew(NodeType node, util::second_t slew);
    void required(NodeType node, util::second_t required);
    void load(NodeType node, util::farad_t load);

    const ophidian::util::second_t arrival(NodeType node) const;
    const ophidian::util::second_t slew(NodeType node) const;
    const ophidian::util::second_t required(NodeType node) const;
    const ophidian::util::farad_t load(NodeType node) const;

private:
    TimingGraph::GraphType::NodeMap<util::second_t> mArrivals;
    TimingGraph::GraphType::NodeMap<util::second_t> mSlews;
    TimingGraph::GraphType::NodeMap<util::second_t> mRequireds;
    TimingGraph::GraphType::NodeMap<util::farad_t>  mLoads;
};


class GraphArcsTiming
{
public:
    using ArcType = TimingGraph::ArcType;

    GraphArcsTiming(const TimingGraph & graph);
    virtual ~GraphArcsTiming();

    void delay(ArcType arc, const util::second_t delay);
    void slew(ArcType arc, const util::second_t delay);

    const ophidian::util::second_t delay(ArcType arc) const;
    const ophidian::util::second_t slew(ArcType arc) const;

private:
    TimingGraph::GraphType::ArcMap<util::second_t> mDelays;
    TimingGraph::GraphType::ArcMap<util::second_t> mSlews;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H
