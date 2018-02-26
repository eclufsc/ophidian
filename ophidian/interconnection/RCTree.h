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

#ifndef OPHIDIAN_INTERCONNECTION_RCTREE_H
#define OPHIDIAN_INTERCONNECTION_RCTREE_H

#include <lemon/list_graph.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace interconnection
{

//! RCTree Class
/*!
   A graph G(V,E), where V is the set of Capacitances and E is the set of Resistances.
 */
class RCTree
{
public:
//    using GraphType = lemon::ListDigraph;
//    using NodeType = GraphType::Node;
//    using EdgeType = GraphType::Arc;

    RCTree();

//private:
//    GraphType mGraph;
//    GraphType::NodeMap<std::string> mNames;
//    GraphType::NodeMap<util::farad_t> mCapacitances;
//    GraphType::ArcMap<util::ohm_t> mResistances;
//    util::farad_t mLumpedCapacitance;
//    std::vector<NodeType> mTaps;

};

}   // namespace timing
}   // namespace ophidian



#endif // OPHIDIAN_TIMING_RCTREE_H
