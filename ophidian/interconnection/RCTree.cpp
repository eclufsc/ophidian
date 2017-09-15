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

#include "RCTree.h"

#include <deque>

namespace ophidian {
namespace interconnection {

RCTree::RCTree() :
    mNames(mGraph), mCapacitances(mGraph), mResistances(mGraph), mLumpedCapacitance(0.0) {

}

RCTree::RCTree(const RCTree &other):
    mNames(mGraph),
    mCapacitances(mGraph),
    mResistances(mGraph),
    mLumpedCapacitance(0.0)
{
    *this = other;
}

RCTree &RCTree::operator=(const RCTree &other)
{
    lemon::GraphCopy<lemon::ListGraph, lemon::ListGraph> cg(other.graph(), mGraph);
    lemon::ListGraph::NodeMap<lemon::ListGraph::Node> nr(other.graph());
    cg.nodeRef(nr);

    lemon::ListGraph::EdgeMap<lemon::ListGraph::Edge> ecr(mGraph);
    cg.edgeCrossRef(ecr);

    cg.nodeMap(other.mNames, mNames);
    cg.nodeMap(other.mCapacitances, mCapacitances);
    cg.edgeMap(other.mResistances, mResistances);
    mLumpedCapacitance = other.mLumpedCapacitance;
    cg.run();

    for(lemon::ListGraph::NodeIt it(mGraph); it != lemon::INVALID; ++it)
        mName2Node[mNames[it]] = it;

    for(auto tap : other.mTaps)
        mTaps.push_back(nr[tap]);

    return *this;
}

RCTree::~RCTree() {
}

void RCTree::tapInsert(RCTree::capacitorID cap)
{
    mTaps.push_back(cap);
}

PackedRCTree RCTree::pack(capacitorID source) const
{
    PackedRCTree result(capacitorCount());

    enum status {
        OPEN, TOUCHED, CLOSED
    };

    graph_t::NodeMap<resistorID> pred(mGraph);
    graph_t::NodeMap<std::size_t> order(mGraph);
    std::vector<capacitorID> reverseOrder(capacitorCount());
    graph_t::NodeMap<status> stat(mGraph, OPEN);
    std::deque<capacitorID> ready;
    ready.push_back(source);
    std::size_t currentOrder{0};
    while(!ready.empty())
    {
        auto cap = ready.front();
        ready.pop_front();
        stat[cap] = CLOSED;
        reverseOrder[currentOrder] = cap;
        order[cap] = currentOrder++;
        for(graph_t::OutArcIt it(mGraph, cap); it != lemon::INVALID; ++it)
        {
            capacitorID target = mGraph.target(it);
            switch(stat[target])
            {
            case OPEN:
                pred[target] = it;
                stat[target] = TOUCHED;
                ready.push_back(target);
                break;
            case TOUCHED:
            case CLOSED:
                break;
            }
        }
    }


    result.pred(0, std::numeric_limits<std::size_t>::max());
    for(currentOrder = 0; currentOrder < reverseOrder.size(); ++currentOrder)
    {
        auto currentCapacitor = reverseOrder[currentOrder];
        result.capacitance(currentOrder, capacitance(currentCapacitor));
        if(currentOrder > 0)
        {
            auto parent = mGraph.oppositeNode(currentCapacitor, pred[currentCapacitor]);
            result.resistance(currentOrder, resistance(pred[currentCapacitor]));
            result.pred(currentOrder, order[parent]);
        }
    }
    for(auto tap : mTaps)
        result.tap(mNames[tap], order[tap]);
    return result;
}

lemon::ListGraph::Node RCTree::capacitorInsert(std::string name) {
    auto result = mName2Node.find(name);
    if (result != mName2Node.end())
        return result->second;
    auto node = mGraph.addNode();
    mNames[node] = name;
    mName2Node[name] = node;
    mCapacitances[node] = util::femtofarad_t(0.0);
    return node;
}

lemon::ListGraph::Edge RCTree::resistorInsert(lemon::ListGraph::Node u,
                                                lemon::ListGraph::Node v, util::kiloohm_t res) {
    auto resistor = mGraph.addEdge(u, v);
    mResistances[resistor] = res;
    return resistor;
}

void RCTree::capacitance(lemon::ListGraph::Node u, util::femtofarad_t cap) {
    mLumpedCapacitance = mLumpedCapacitance - mCapacitances[u];
    mCapacitances[u] = cap;
    mLumpedCapacitance = mLumpedCapacitance + mCapacitances[u];
}

PackedRCTree::PackedRCTree(std::size_t nodeCount) :
    mPred(nodeCount, -1),
    mResistances(nodeCount),
    mCapacitances(nodeCount)
{

}

PackedRCTree::~PackedRCTree()
{

}

void PackedRCTree::pred(std::size_t i, std::size_t pred)
{
    mPred[i] = pred;
}

void PackedRCTree::capacitance(std::size_t i, util::femtofarad_t cap)
{
    mCapacitances[i] = cap;
}

void PackedRCTree::resistance(std::size_t i, util::kiloohm_t res)
{
    mResistances[i] = res;
}

void PackedRCTree::tap(const std::string &name, std::size_t value)
{
    mTaps[name] = value;
}

} /* namespace timing */
} /* namespace ophidian */

