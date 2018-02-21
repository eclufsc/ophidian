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

#ifndef OPHIDIAN_TIMING_TIMINGGRAPH_H
#define OPHIDIAN_TIMING_TIMINGGRAPH_H

#include <exception>
#include <lemon/list_graph.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/timing/TimingArcs.h>
#include <ophidian/timing/Library.h>

namespace ophidian
{
namespace timing
{

enum class node_types
{
    Rise, Fall
};

enum class edge_types
{
    TimingArc, Net
};

class Edge : public Arc, public circuit::Net
{
};

class TimingGraph
{
public:
    using graph_t = lemon::ListDigraph;
    using node_t = graph_t::Node;
    using edge_t = graph_t::Arc;

    TimingGraph(const circuit::Netlist & netlist);
    ~TimingGraph();

    const graph_t & graph();
    size_t nodesAmount();
    size_t edgesAmount();

    node_t riseNodeCreate(const circuit::Pin & pin);
    node_t riseNode(const circuit::Pin & pin);
    node_t fallNodeCreate(const circuit::Pin & pin);
    node_t fallNode(const circuit::Pin & pin);

    edge_t edgeCreate(const node_t & from, const node_t & to, const Edge & edge, const edge_types & type);

    node_types type(const node_t & node);
    edge_types type(const edge_t & edge);

    node_t source(const edge_t & edge);
    node_t target(const edge_t & edge);

    graph_t::OutArcIt outEdge(const node_t & node);
    graph_t::InArcIt inEdge(const node_t & node);

    circuit::Pin entity(const node_t & node);
    circuit::Net entity(circuit::Net, const edge_t & edge);
    Arc entity(Arc, const edge_t & edge);
    Edge entity(Edge, const edge_t & edge);

    template <class Iterator>
    void destroy(const Iterator begin, const Iterator end)
    {
        for(Iterator it{begin}; it != end; ++it)
            mGraph.erase(*it);
    }

private:
    node_t nodeCreate(const circuit::Pin & pin, const node_types & type, entity_system::Property<circuit::Pin, node_t> & map);

    graph_t mGraph;
    graph_t::NodeMap<circuit::Pin> mPins;
    graph_t::NodeMap<node_types> mNodeTypes;
    graph_t::ArcMap<Edge> mEdges;
    graph_t::ArcMap<edge_types> mEdgeTypes;

    entity_system::Property<circuit::Pin, node_t> mRiseNodes;
    entity_system::Property<circuit::Pin, node_t> mFallNodes;
};

} // namespace timing
} // namespace ophidian


#endif // OPHIDIAN_TIMING_TIMINGGRAPH_H
