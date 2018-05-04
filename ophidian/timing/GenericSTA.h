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

#ifndef OPHIDIAN_TIMING_GENERICSTA_H
#define OPHIDIAN_TIMING_GENERICSTA_H

#include <limits>
#include <lemon/path.h>
#include <lemon/euler.h>
#include <ophidian/util/Units.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingGraphCondensation.h>
#include <ophidian/timingdriven_placement/RCTree.h>
#include <ophidian/parser/SDCParser.h>

namespace ophidian
{
namespace timing
{

struct Optimistic {
    // Class member types
    using timing_type = util::second_t;

    timing_type operator()(const timing_type &a, const timing_type &b) const {
        return std::min(a, b);
    }
    timing_type inverted(const timing_type &a, const timing_type &b) const {
        return std::max(a, b);
    }
    static double slackSignal() {
        return -1.0;
    }
    static timing_type best() {
        return std::numeric_limits<timing_type>::max();
    }
    static timing_type worst() {
        return -std::numeric_limits<timing_type>::max();
    }
};


struct Pessimistic {
    // Class member types
    using timing_type = util::second_t;

    timing_type operator()(const timing_type &a, const timing_type &b) const {
        return std::max(a, b);
    }
    timing_type inverted(const timing_type &a, const timing_type &b) const {
        return std::min(a, b);
    }
    static double slackSignal() {
        return 1.0;
    }
    static timing_type best() {
        return -std::numeric_limits<timing_type>::max();
    }
    static timing_type worst() {
        return std::numeric_limits<timing_type>::max();
    }
};


struct TimingData {
    const Library&   mLibrary;
    GraphNodesTiming mNodesTiming;
    GraphArcsTiming  mArcsTiming;

    TimingData() = delete;

    TimingData(const TimingData&) = delete;
    TimingData& operator=(const TimingData&) = delete;

    TimingData(TimingData&&) = default;
    TimingData& operator=(TimingData&&) = default;

    TimingData(const Library& lib, const TimingGraph& graph);
};


struct GraphAndTopology {
    const TimingGraph&                              mGraph;
    circuit::Netlist&                               mNetlist;
    std::vector<TimingGraph::NodeType>              mSorted;
    std::vector<std::vector<TimingGraph::NodeType>> mLevels;
    std::vector<TimingGraph::NodeType>              mSortedDrivers;

    GraphAndTopology() = delete;

    GraphAndTopology(const GraphAndTopology&) = delete;
    GraphAndTopology& operator=(const GraphAndTopology&) = delete;

    GraphAndTopology(GraphAndTopology&&) = default;
    GraphAndTopology& operator=(GraphAndTopology&&) = default;

    GraphAndTopology(const TimingGraph& graph,
                     circuit::Netlist& netlist,
                     standard_cell::StandardCells& stdCells,
                     circuit::LibraryMapping& libraryMapping);
};

template <class WireDelayModel, class MergeStrategy>
class GenericSTA
{
public:
    // Class member types
    template <class T> using container_type   = std::vector<T>;
    template <class T> using node_mapper_type = timingdriven_placement::RCTree::graph_type::NodeMap<T>;

    using slew_type                           = util::second_t;
    using pico_slew_type                      = util::picosecond_t;
    using capacitance_type                    = util::farad_t;
    using slew_map_type                       = node_mapper_type<slew_type>;
    using capacitance_map_type                = node_mapper_type<capacitance_type>;

    GenericSTA(TimingData & data,
               GraphAndTopology & topology,
               entity_system::Property<circuit::Net, timingdriven_placement::RCTree> & rctrees) :
        mTimingData(data),
        mTopology(&topology),
        mRCTrees(rctrees)
    {
    }

    void topology(GraphAndTopology & topology)
    {
        mTopology = &topology;
    }

    void constraints(const parser::DesignConstraints & dc, const circuit::LibraryMapping libraryMapping)
    {
        mTimingData.mNodesTiming.arrival(
                    mTopology->mGraph.riseNode(mTopology->mNetlist.find(circuit::Pin(), dc.mClock.portName)),
                    slew_type(0,0));
        mTimingData.mNodesTiming.arrival(
                    mTopology->mGraph.fallNode(mTopology->mNetlist.find(circuit::Pin(), dc.mClock.portName)),
                    slew_type(0,0));

        for(auto & i : dc.mInputDelays)
        {
            auto pin = mTopology->mNetlist.find(circuit::Pin(), i.portName);
            mTimingData.mNodesTiming.arrival(mTopology->mGraph.riseNode(pin), slew_type(pico_slew_type(i.delay)));
            mTimingData.mNodesTiming.arrival(mTopology->mGraph.fallNode(pin), slew_type(pico_slew_type(i.delay)));
        }

        for(auto & i : dc.mInputDrivers)
        {
            auto pin = mTopology->mNetlist.find(circuit::Pin(), i.portName);
            mTimingData.mNodesTiming.slew(mTopology->mGraph.riseNode(pin), slew_type(pico_slew_type(i.slewRise)));
            mTimingData.mNodesTiming.slew(mTopology->mGraph.fallNode(pin), slew_type(pico_slew_type(i.slewFall)));
        }

        for(lemon::ListDigraph::NodeIt node(mTopology->mGraph.graph()); node != lemon::INVALID; ++node)
        {
            if(mTimingData.mLibrary.pinClock(libraryMapping.pinStdCell(mTopology->mGraph.entity(node))))
                mTimingData.mNodesTiming.required(node, MergeStrategy::worst());
            else if(lemon::countOutArcs(mTopology->mGraph.graph(), node) == 0)
                mTimingData.mNodesTiming.required(node, mMerge(slew_type(0.0), slew_type(pico_slew_type(dc.mClock.period))));
        }
    }

    slew_type riseArrival(const circuit::Pin pin) const
    {
        return mTimingData.mNodesTiming.arrival(mTopology->mGraph.riseNode(pin));
    }

    slew_type fallArrival(const circuit::Pin pin) const
    {
        return mTimingData.mNodesTiming.arrival(mTopology->mGraph.fallNode(pin));
    }

    slew_type riseSlew(const circuit::Pin pin) const
    {
        return mTimingData.mNodesTiming.slew(mTopology->mGraph.riseNode(pin));
    }

    slew_type fallSlew(const circuit::Pin pin) const
    {
        return mTimingData.mNodesTiming.slew(mTopology->mGraph.fallNode(pin));
    }

    slew_type riseSlack(const circuit::Pin pin) const
    {
        auto node = mTopology->mGraph.riseNode(pin);
        return MergeStrategy::slack_signal() *
               (mTimingData.mNodesTiming.required(node) - mTimingData.mNodesTiming.arrival(node));
    }

    slew_type fallSlack(const circuit::Pin pin) const
    {
        auto node = mTopology->mGraph.fallNode(pin);
        return MergeStrategy::slack_signal() *
               (mTimingData.mNodesTiming.required(node) - mTimingData.mNodesTiming.arrival(node));
    }

    void updateATS()
    {
        for(auto & level : mTopology->mLevels)
        {
            std::size_t i;
            for(i = 0; i < level.size(); ++i)
            {
                auto node = level[i];
                if(lemon::countInArcs(mTopology->mGraph.graph(), node) != 0)
                {
                    auto pin = mTopology->mGraph.entity(node);
                    auto net = mTopology->mNetlist.net(pin);
                    auto & tree = mRCTrees[net];

                    node_mapper_type<slew_type>        slews(tree.g());
                    node_mapper_type<slew_type>        delays(tree.g());
                    node_mapper_type<capacitance_type> ceffs(tree.g());

                    WireDelayModel calculator;
                    calculator.delay_map(delays);
                    calculator.slew_map(slews);
                    calculator.ceff_map(ceffs);

                    std::function<slew_type(capacitance_type)> sCalculator = std::bind(&GenericSTA::computeSlew, this, node, std::placeholders::_1);

                    capacitance_type load = calculator.simulate(sCalculator, tree);

                    mTimingData.mNodesTiming.load(node, load);
                    mTimingData.mNodesTiming.slew(node, slews[tree.source()]);

                    slew_type worstArrival = MergeStrategy::best();
                    switch(mTopology->mGraph.property(node))
                    {
                    case TimingGraph::NodeProperty::Rise:
                        for(lemon::ListDigraph::InArcIt it(mTopology->mGraph.graph(), node); it != lemon::INVALID; ++it)
                        {
                            TimingArc tarc = mTopology->mGraph.entity(TimingArc(), it);
                            auto edgeSource = mTopology->mGraph.source(it);
                            auto arcDelay = mTimingData.mLibrary.computeRiseDelay(tarc, load, mTimingData.mNodesTiming.slew(edgeSource));
                            auto arcSlew = mTimingData.mLibrary.computeRiseSlews(tarc, load, mTimingData.mNodesTiming.slew(edgeSource));
                            mTimingData.mArcsTiming.delay(it, arcDelay);
                            mTimingData.mArcsTiming.slew(it, arcSlew);
                            worstArrival = mMerge(worstArrival, mTimingData.mNodesTiming.arrival(edgeSource) + arcDelay);
                        }
                        break;

                    case TimingGraph::NodeProperty::Rise:
                        for(lemon::ListDigraph::InArcIt it(mTopology->mGraph.graph(), node); it != lemon::INVALID; ++it)
                        {
                            auto tarc = mTopology->mGraph.entity(TimingArc(), it) ;
                            auto edgeSource = mTopology->mGraph.source(it);
                            auto arcDelay = mTimingData.mLibrary.computeFallDelay(tarc, load, mTimingData.mNodesTiming.slew(edgeSource));
                            auto arcSlew = mTimingData.mLibrary.computeFallSlews(tarc, load, mTimingData.mNodesTiming.slew(edgeSource));
                            mTimingData.mArcsTiming.delay(it, arcDelay);
                            mTimingData.mArcsTiming.slew(it, arcSlew);
                            worstArrival = mMerge(worstArrival, mTimingData.mNodesTiming.arrival(edgeSource) + arcDelay);
                        }
                        break;
                    }

                    mTimingData.mNodesTiming.arrival(node, worstArrival);
                    for(lemon::ListDigraph::OutArcIt arc(mTopology->mGraph.graph(), node); arc != lemon::INVALID; ++arc)
                    {
                        auto arc_target = mTopology->mGraph.target(arc);
                        auto target_pin = mTopology->mGraph.entity(arc_target);
                        auto target_capacitor = tree.capacitor(mTopology->mNetlist.name(target_pin));
                        mTimingData.mArcsTiming.slew(arc, slews[target_capacitor]);
                        mTimingData.mArcsTiming.delay(arc, delays[target_capacitor]);
                        mTimingData.mNodesTiming.slew(arc_target, mTimingData.mArcsTiming.slew(arc));
                        mTimingData.mNodesTiming.arrival(arc_target, mTimingData.mNodesTiming.arrival(node) + mTimingData.mArcsTiming.delay(arc));
                    }
                }
            }
        }
    }

    void updateRTS()
    {
        for(auto node_it = mTopology->mSorted.rbegin(); node_it != mTopology->mSorted.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(mTopology->mGraph.graph(), node) > 0)
            {
                slew_type required = MergeStrategy::worst();
                for(lemon::ListDigraph::OutArcIt arc(mTopology->mGraph.graph(), node); arc != lemon::INVALID; ++arc)
                    required = mMerge.inverted(required,
                                mTimingData.mNodesTiming.required(mTopology->mGraph.target(arc)) - mTimingData.mArcsTiming.delay(arc));
                mTimingData.mNodesTiming.required(node, required);
            }
        }
    }

    lemon::Path<TimingGraph::graph_type> criticalPath() const
    {
        lemon::Path<TimingGraph::graph_type> cp;
        slew_type worstSlack = std::numeric_limits<slew_type>::max();
        TimingGraph::NodeType worst_PO;

        for(auto node_it = mTopology->mSorted.rbegin(); node_it != mTopology->mSorted.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(mTopology->mGraph.graph(), node) == 0)
            {
                slew_type current_PO_slack = MergeStrategy::slack_signal() *
                        (mTimingData.mNodesTiming.required(node) - mTimingData.mNodesTiming.arrival(node));
                if(current_PO_slack < worstSlack)
                {
                    worstSlack = current_PO_slack;
                    worst_PO = node;
                }
            }
        }

        lemon::ListDigraph::Node currentNode = worst_PO;
        lemon::ListDigraph::Node nextNode = currentNode;

        while(nextNode != lemon::INVALID)
        {
            currentNode = nextNode;
            nextNode = lemon::INVALID;
            lemon::ListDigraph::Arc worst_arc = lemon::INVALID;
            slew_type worstSlack_input = std::numeric_limits<slew_type>::max();
            for(lemon::ListDigraph::InArcIt in(mTopology->mGraph.graph(), currentNode); in != lemon::INVALID; ++in)
            {
                auto source = mTopology->mGraph.graph().source(in);
                slew_type slack = MergeStrategy::slack_signal() *
                        (mTimingData.mNodesTiming.required(source) - mTimingData.mNodesTiming.arrival(source));

                if(slack < worstSlack_input)
                {
                    worstSlack_input = slack;
                    worst_arc = in;
                    nextNode = source;
                }
            }
            if(worst_arc != lemon::INVALID)
                cp.addFront(worst_arc);
        }

        return cp;
    }

private:
    slew_type computeSlew(TimingGraph::NodeType node, capacitance_type load) const
    {
        slew_type worstSlew = MergeStrategy::best();

        if(lemon::countInArcs(mTopology->mGraph.graph(), node) == 0)
            return mTimingData.mNodesTiming.slew(node);

        switch(mTopology->mGraph.property(node))
        {
        case TimingGraph::NodeProperty::Rise:
            for(lemon::ListDigraph::InArcIt it(mTopology->mGraph.graph(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = mTopology->mGraph.entity(TimingArc(), it);
                worstSlew = mMerge(worstSlew,
                                   mTimingData.mLibrary.computeRiseSlews(tarc, load, mTimingData.mNodesTiming.slew(mTopology->mGraph.source(it))));
            }
            break;

        case TimingGraph::NodeProperty::Fall:
            for(lemon::ListDigraph::InArcIt it(mTopology->mGraph.graph(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = mTopology->mGraph.entity(TimingArc(), it);
                worstSlew = mMerge(worstSlew,
                                   mTimingData.mLibrary.computeFallSlews(tarc, load, mTimingData.mNodesTiming.slew(mTopology->mGraph.source(it))));
            }
            break;
        }

        return worstSlew;
    }

    TimingData&                                                            mTimingData;
    GraphAndTopology*                                                      mTopology;
    entity_system::Property<circuit::Net, timingdriven_placement::RCTree>& mRCTrees;
    MergeStrategy                                                          mMerge;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_GENERICSTA_H
