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

#ifndef OPHIDIAN_TIMING_ELMORE_H
#define OPHIDIAN_TIMING_ELMORE_H

#include <ophidian/timingdriven_placement/RCTree.h>

namespace ophidian
{
namespace timing
{

class Elmore
{
public:
    using GraphRCTreeType = timingdriven_placement::RCTree::GraphType;
    using CapacitorRCTree = timingdriven_placement::RCTree::Capacitor;
    using ResistorRCTree = timingdriven_placement::RCTree::Resistor;

    Elmore(const timingdriven_placement::RCTree & tree, const CapacitorRCTree & source);
    virtual ~Elmore();

    void update();

    util::second_t at(const CapacitorRCTree cap) const;
    const GraphRCTreeType::NodeMap<std::pair<CapacitorRCTree, ResistorRCTree>> & pred() const;
    const std::vector<CapacitorRCTree> & order() const;

private:
    const timingdriven_placement::RCTree & mTree;
    GraphRCTreeType::NodeMap<util::second_t> mElmoreDelay;
    GraphRCTreeType::NodeMap<util::farad_t> mDownstreamCapacitance;
    GraphRCTreeType::NodeMap<std::pair<CapacitorRCTree, ResistorRCTree>> mPred;
    std::vector<CapacitorRCTree> mOrder;
    const CapacitorRCTree mSource;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_ELMORE_H

