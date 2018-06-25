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
#include <ophidian/timing/TimingData.h>
#include <ophidian/timingdriven_placement/RCTree.h>
#include <ophidian/parser/SDCParser.h>

namespace ophidian
{
namespace timing
{

struct Optimistic {
    // Class member types
    using time_unit_type = util::second_t;

    time_unit_type operator()(const time_unit_type &a, const time_unit_type &b) const {
        return std::min(a, b);
    }
    time_unit_type inverted(const time_unit_type &a, const time_unit_type &b) const {
        return std::max(a, b);
    }
    static double slack_signal() {
        return -1.0;
    }
    static time_unit_type best() {
        return std::numeric_limits<time_unit_type>::max();
    }
    static time_unit_type worst() {
        return -std::numeric_limits<time_unit_type>::max();
    }
};


struct Pessimistic {
    // Class member types
    using time_unit_type = util::second_t;

    time_unit_type operator()(const time_unit_type &a, const time_unit_type &b) const {
        return std::max(a, b);
    }
    time_unit_type inverted(const time_unit_type &a, const time_unit_type &b) const {
        return std::min(a, b);
    }
    static double slack_signal() {
        return 1.0;
    }
    static time_unit_type best() {
        return -std::numeric_limits<time_unit_type>::max();
    }
    static time_unit_type worst() {
        return std::numeric_limits<time_unit_type>::max();
    }
};

struct GraphAndTopology
{
    template <class T> using container_type = std::vector<T>;

    using netlist_type                  = circuit::Netlist;
    using library_mapping_type          = circuit::LibraryMapping;
    using standard_cells_type           = standard_cell::StandardCells;
    using library_type                  = Library;
    using timing_graph_type             = TimingGraph;
    using sorted_container_type         = container_type<timing_graph_type::node_type>;
    using levels_container_type         = container_type<container_type<timing_graph_type::node_type>>;
    using sorted_drivers_container_type = container_type<timing_graph_type::node_type>;

    GraphAndTopology() = delete;

    GraphAndTopology(const GraphAndTopology&) = delete;
    GraphAndTopology& operator=(const GraphAndTopology&) = delete;

    GraphAndTopology(GraphAndTopology&&) = default;
    GraphAndTopology& operator=(GraphAndTopology&&) = default;

    GraphAndTopology(const timing_graph_type& graph,
                     netlist_type& netlist,
                     standard_cells_type& stdCells,
                     library_mapping_type& libraryMapping);

    const timing_graph_type&      m_graph;
    netlist_type&                 m_netlist;
    sorted_container_type         m_sorted;
    levels_container_type         m_levels;
    sorted_drivers_container_type m_sorted_drivers;
};

template <class WireDelayModel, class MergeStrategy>
class GenericSTA
{
public:
    // Class member types
    template <class T> using container_type = std::vector<T>;
    template <class T> using node_map_type  = timingdriven_placement::RCTree::graph_type::NodeMap<T>;

    using design_constraints_type           = parser::DesignConstraints;
    using library_mapping_type              = circuit::LibraryMapping;
    using timing_data_type                  = TimingData;
    using timing_graph_type                 = TimingGraph;
    using topology_type                     = GraphAndTopology;
    using net_to_rctree_property_type       = entity_system::Property<circuit::Net, timingdriven_placement::RCTree>;

    using slew_unit_type                    = util::second_t;
    using pico_slew_unit_type               = util::picosecond_t;
    using capacitance_unit_type             = util::farad_t;
    using pin_entity_type                   = circuit::Pin;
    using slew_map_type                     = node_map_type<slew_unit_type>;
    using capacitance_map_type              = node_map_type<capacitance_unit_type>;

    GenericSTA(timing_data_type & data,
               topology_type & topology,
               net_to_rctree_property_type & rctrees) :
        m_timing_data(data),
        m_topology(&topology),
        m_rctrees(rctrees)
    {
    }

    void topology(topology_type & topology)
    {
        m_topology = &topology;
    }

    void constraints(const design_constraints_type & dc, const library_mapping_type & libraryMapping)
    {
        m_timing_data.arrival(
                    m_topology->m_graph.riseNode(m_topology->m_netlist.find(pin_entity_type(), dc.mClock.portName)),
                    slew_unit_type(0));
        m_timing_data.arrival(
                    m_topology->m_graph.fallNode(m_topology->m_netlist.find(pin_entity_type(), dc.mClock.portName)),
                    slew_unit_type(0));

        for(auto & i : dc.mInputDelays)
        {
            auto pin = m_topology->m_netlist.find(pin_entity_type(), i.portName);
            m_timing_data.arrival(m_topology->m_graph.riseNode(pin), slew_unit_type(pico_slew_unit_type(i.delay)));
            m_timing_data.arrival(m_topology->m_graph.fallNode(pin), slew_unit_type(pico_slew_unit_type(i.delay)));
        }

        for(auto & i : dc.mInputDrivers)
        {
            auto pin = m_topology->m_netlist.find(pin_entity_type(), i.portName);
            m_timing_data.slew(m_topology->m_graph.riseNode(pin), slew_unit_type(pico_slew_unit_type(i.slewRise)));
            m_timing_data.slew(m_topology->m_graph.fallNode(pin), slew_unit_type(pico_slew_unit_type(i.slewFall)));
        }

        for(lemon::ListDigraph::NodeIt node(m_topology->m_graph.graph()); node != lemon::INVALID; ++node)
        {
            if(m_timing_data.library().pinClock(libraryMapping.pinStdCell(m_topology->m_graph.entity(node))))
                m_timing_data.required(node, MergeStrategy::worst());
            else if(lemon::countOutArcs(m_topology->m_graph.graph(), node) == 0)
                m_timing_data.required(node, m_merge(slew_unit_type(0.0), slew_unit_type(pico_slew_unit_type(dc.mClock.period))));
        }
    }

    slew_unit_type rise_arrival(const pin_entity_type& pin) const
    {
        return m_timing_data.arrival(m_topology->m_graph.riseNode(pin));
    }

    slew_unit_type fall_arrival(const pin_entity_type& pin) const
    {
        return m_timing_data.arrival(m_topology->m_graph.fallNode(pin));
    }

    slew_unit_type rise_slew(const pin_entity_type& pin) const
    {
        return m_timing_data.slew(m_topology->m_graph.riseNode(pin));
    }

    slew_unit_type fall_slew(const pin_entity_type& pin) const
    {
        return m_timing_data.slew(m_topology->m_graph.fallNode(pin));
    }

    slew_unit_type rise_slack(const pin_entity_type& pin) const
    {
        auto node = m_topology->m_graph.riseNode(pin);
        return MergeStrategy::slack_signal() *
               (m_timing_data.required(node) - m_timing_data.arrival(node));
    }

    slew_unit_type fall_slack(const pin_entity_type& pin) const
    {
        auto node = m_topology->m_graph.fallNode(pin);
        return MergeStrategy::slack_signal() *
               (m_timing_data.required(node) - m_timing_data.arrival(node));
    }

    void update_ats()
    {
        for(auto & level : m_topology->m_levels)
        {
            std::size_t i;
            for(i = 0; i < level.size(); ++i)
            {
                auto node = level[i];
                if(lemon::countInArcs(m_topology->m_graph.graph(), node) != 0)
                {
                    auto pin = m_topology->m_graph.entity(node);
                    auto net = m_topology->m_netlist.net(pin);
                    auto & tree = m_rctrees[net];

                    slew_map_type        slews(tree.g());
                    slew_map_type        delays(tree.g());
                    capacitance_map_type ceffs(tree.g());

                    WireDelayModel calculator;
                    calculator.delays(delays);
                    calculator.slews(slews);
                    calculator.ceff(ceffs);

                    std::function<slew_unit_type(capacitance_unit_type)> sCalculator = std::bind(&GenericSTA::compute_slew, this, node, std::placeholders::_1);

                    capacitance_unit_type load = calculator.simulate(sCalculator, tree);

                    m_timing_data.load(node, load);
                    m_timing_data.slew(node, slews[tree.source()]);

                    slew_unit_type worstArrival = MergeStrategy::best();
                    switch(m_topology->m_graph.property(node))
                    {
                    case timing_graph_type::NodeProperty::Rise:
                        for(lemon::ListDigraph::InArcIt it(m_topology->m_graph.graph(), node); it != lemon::INVALID; ++it)
                        {
                            auto tarc = m_topology->m_graph.entity(TimingArcs::timing_arc_entity_type(), it);
                            auto edgeSource = m_topology->m_graph.source(it);
                            auto arcDelay = m_timing_data.library().computeRiseDelay(tarc, load, m_timing_data.slew(edgeSource));
                            auto arcSlew = m_timing_data.library().computeRiseSlews(tarc, load, m_timing_data.slew(edgeSource));
                            m_timing_data.delay(it, arcDelay);
                            m_timing_data.slew(it, arcSlew);
                            worstArrival = m_merge(worstArrival, m_timing_data.arrival(edgeSource) + arcDelay);
                        }
                        break;

                    case timing_graph_type::NodeProperty::Fall:
                        for(lemon::ListDigraph::InArcIt it(m_topology->m_graph.graph(), node); it != lemon::INVALID; ++it)
                        {
                            auto tarc = m_topology->m_graph.entity(TimingArcs::timing_arc_entity_type(), it) ;
                            auto edgeSource = m_topology->m_graph.source(it);
                            auto arcDelay = m_timing_data.library().computeFallDelay(tarc, load, m_timing_data.slew(edgeSource));
                            auto arcSlew = m_timing_data.library().computeFallSlews(tarc, load, m_timing_data.slew(edgeSource));
                            m_timing_data.delay(it, arcDelay);
                            m_timing_data.slew(it, arcSlew);
                            worstArrival = m_merge(worstArrival, m_timing_data.arrival(edgeSource) + arcDelay);
                        }
                        break;
                    }

                    m_timing_data.arrival(node, worstArrival);
                    for(lemon::ListDigraph::OutArcIt arc(m_topology->m_graph.graph(), node); arc != lemon::INVALID; ++arc)
                    {
                        auto arc_target = m_topology->m_graph.target(arc);
                        auto target_pin = m_topology->m_graph.entity(arc_target);
                        auto target_capacitor = tree.capacitor(m_topology->m_netlist.name(target_pin));
                        m_timing_data.slew(arc, slews[target_capacitor]);
                        m_timing_data.delay(arc, delays[target_capacitor]);
                        m_timing_data.slew(arc_target, m_timing_data.slew(arc));
                        m_timing_data.arrival(arc_target, m_timing_data.arrival(node) + m_timing_data.delay(arc));
                    }
                }
            }
        }
    }

    void update_rts()
    {
        for(auto node_it = m_topology->m_sorted.rbegin(); node_it != m_topology->m_sorted.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(m_topology->m_graph.graph(), node) > 0)
            {
                slew_unit_type required = MergeStrategy::worst();
                for(lemon::ListDigraph::OutArcIt arc(m_topology->m_graph.graph(), node); arc != lemon::INVALID; ++arc)
                    required = m_merge.inverted(required,
                                m_timing_data.required(m_topology->m_graph.target(arc)) - m_timing_data.delay(arc));
                m_timing_data.required(node, required);
            }
        }
    }

    lemon::Path<timing_graph_type::graph_type> critical_path() const
    {
        lemon::Path<timing_graph_type::graph_type> cp;
        slew_unit_type worstSlack = std::numeric_limits<slew_unit_type>::max();
        timing_graph_type::node_type worst_PO;

        for(auto node_it = m_topology->m_sorted.rbegin(); node_it != m_topology->m_sorted.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(m_topology->m_graph.graph(), node) == 0)
            {
                slew_unit_type current_PO_slack = MergeStrategy::slack_signal() *
                        (m_timing_data.required(node) - m_timing_data.arrival(node));
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
            slew_unit_type worstSlack_input = std::numeric_limits<slew_unit_type>::max();
            for(lemon::ListDigraph::InArcIt in(m_topology->m_graph.graph(), currentNode); in != lemon::INVALID; ++in)
            {
                auto source = m_topology->m_graph.graph().source(in);
                slew_unit_type slack = MergeStrategy::slack_signal() *
                        (m_timing_data.required(source) - m_timing_data.arrival(source));

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
    slew_unit_type compute_slew(timing_graph_type::node_type node, capacitance_unit_type load) const
    {
        slew_unit_type worstSlew = MergeStrategy::best();

        if(lemon::countInArcs(m_topology->m_graph.graph(), node) == 0)
            return m_timing_data.slew(node);

        switch(m_topology->m_graph.property(node))
        {
        case timing_graph_type::NodeProperty::Rise:
            for(lemon::ListDigraph::InArcIt it(m_topology->m_graph.graph(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_topology->m_graph.entity(TimingArcs::timing_arc_entity_type(), it);
                worstSlew = m_merge(worstSlew,
                                   m_timing_data.library().computeRiseSlews(tarc, load, m_timing_data.slew(m_topology->m_graph.source(it))));
            }
            break;

        case timing_graph_type::NodeProperty::Fall:
            for(lemon::ListDigraph::InArcIt it(m_topology->m_graph.graph(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_topology->m_graph.entity(TimingArcs::timing_arc_entity_type(), it);
                worstSlew = m_merge(worstSlew,
                                   m_timing_data.library().computeFallSlews(tarc, load, m_timing_data.slew(m_topology->m_graph.source(it))));
            }
            break;
        }

        return worstSlew;
    }

    timing_data_type&            m_timing_data;
    topology_type*               m_topology;
    net_to_rctree_property_type& m_rctrees;
    MergeStrategy                m_merge;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_GENERICSTA_H
