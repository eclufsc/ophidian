/*
 * Copyright 2016 Ophidian
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

#include "../timing/library.h"
#include "../timing/graph.h"
#include "../netlist/netlist.h"
#include "../interconnection/rc_tree.h"

#include <lemon/connectivity.h>

#include <boost/units/limits.hpp>
#include <boost/units/cmath.hpp>
#include <functional>

#include "ceff.h"
#include "design_constraints.h"

#include "graph_nodes_timing.h"
#include "graph_arcs_timing.h"

#include <omp.h>

#ifndef OPHIDIAN_TIMING_GENERIC_STA_H
#define OPHIDIAN_TIMING_GENERIC_STA_H

namespace ophidian {
namespace timing {

struct optimistic {
    using TimingType = boost::units::quantity< boost::units::si::time >;

    TimingType operator()(const TimingType &a, const TimingType &b) const {
        return std::min(a, b);
    }
    TimingType inverted(const TimingType &a, const TimingType &b) const {
        return std::max(a, b);
    }
    static double slack_signal() {
        return -1.0;
    }
    static TimingType best() {
        return std::numeric_limits<TimingType >::infinity();
    }
    static TimingType worst() {
        return -std::numeric_limits<TimingType >::infinity();
    }
};


struct pessimistic {
    using TimingType = boost::units::quantity< boost::units::si::time >;

    TimingType operator()(const TimingType &a, const TimingType &b) const {
        return std::max(a, b);
    }
    TimingType inverted(const TimingType &a, const TimingType &b) const {
        return std::min(a, b);
    }
    static double slack_signal() {
        return 1.0;
    }
    static TimingType best() {
        return -std::numeric_limits<TimingType >::infinity();
    }
    static TimingType worst() {
        return std::numeric_limits<TimingType >::infinity();
    }
};


struct timing_data {
    const library & lib;
    graph_nodes_timing nodes;
    graph_arcs_timing arcs;


    timing_data(const library & lib, const graph& g):
        lib(lib),
        nodes(g.G()),
        arcs(g.G())
    {

    }

};


struct graph_and_topology {
    const graph & g;
    const netlist::netlist & netlist;
    std::vector<lemon::ListDigraph::Node> sorted;
    std::vector< std::vector<lemon::ListDigraph::Node> > levels;
    std::vector<lemon::ListDigraph::Node> sorted_drivers;
    graph_and_topology(const graph & G, const netlist::netlist & netlist, const library & lib);

};


struct test_calculator {

    const graph_and_topology & topology;
    timing_data & early;
    timing_data & late;
    boost::units::quantity< boost::units::si::time > clock_period;

    void compute_tests();


};

template <class WireDelayModel, class MergeStrategy>
class generic_sta
{
    using SlewType = boost::units::quantity< boost::units::si::time >;
    using CapacitanceType = boost::units::quantity< boost::units::si::capacitance >;



    timing_data & m_timing;
    graph_and_topology * m_topology;
    const entity_system::vector_property< interconnection::packed_rc_tree > & m_rc_trees;
    MergeStrategy m_merge;

    SlewType compute_slew(lemon::ListDigraph::Node node, CapacitanceType load) const {
        SlewType worst_slew = MergeStrategy::best();
        if(lemon::countInArcs(m_topology->g.G(), node) == 0) // PI without driver
            return m_timing.nodes.slew(node);
        switch(m_topology->g.node_edge(node))
        {
        case edges::RISE:
            for(lemon::ListDigraph::InArcIt it(m_topology->g.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_topology->g.edge_entity(it) ;
                worst_slew = m_merge(worst_slew, m_timing.lib.timing_arc_rise_slew(tarc).compute(load, m_timing.nodes.slew(m_topology->g.edge_source(it))));
            }
            break;
        case edges::FALL:
            for(lemon::ListDigraph::InArcIt it(m_topology->g.G(), node); it != lemon::INVALID; ++it)
            {
                auto tarc = m_topology->g.edge_entity(it) ;
                worst_slew = m_merge(worst_slew, m_timing.lib.timing_arc_fall_slew(tarc).compute(load, m_timing.nodes.slew(m_topology->g.edge_source(it))));
            }
            break;
        }
        return worst_slew;
    }

public:
    generic_sta( timing_data & timing, graph_and_topology & topology, const entity_system::vector_property< interconnection::packed_rc_tree > & rc_trees) :
        m_timing(timing),
        m_topology(&topology),
        m_rc_trees(rc_trees)
    {




    }


    void topology(graph_and_topology & topology)
    {
        m_topology = &topology;
    }



    void set_constraints(const design_constraints & dc)
    {

        using namespace boost::units;
        using namespace boost::units::si;

        m_timing.nodes.arrival( m_topology->g.rise_node(m_topology->netlist.pin_by_name(dc.clock.port_name)), 0.0*seconds );
        m_timing.nodes.arrival( m_topology->g.fall_node(m_topology->netlist.pin_by_name(dc.clock.port_name)), 0.0*seconds );

        for(auto & i : dc.input_delays)
        {
            auto pin = m_topology->netlist.pin_by_name(i.port_name);
            m_timing.nodes.arrival( m_topology->g.rise_node(pin), quantity<si::time>(i.delay*pico*seconds) );
            m_timing.nodes.arrival( m_topology->g.fall_node(pin), quantity<si::time>(i.delay*pico*seconds) );
        }

        for(auto & i : dc.input_drivers)
        {
            auto pin = m_topology->netlist.pin_by_name(i.port_name);
            m_timing.nodes.slew( m_topology->g.rise_node(pin), quantity<si::time>(i.slew_rise*pico*seconds) );
            m_timing.nodes.slew( m_topology->g.fall_node(pin), quantity<si::time>(i.slew_fall*pico*seconds) );
        }


        for(lemon::ListDigraph::NodeIt node(m_topology->g.G()); node != lemon::INVALID; ++node)
        {
            if(m_timing.lib.pin_clock_input(m_topology->netlist.pin_std_cell(m_topology->g.pin(node))))
                m_timing.nodes.required( node, MergeStrategy::worst() );
            else if(lemon::countOutArcs(m_topology->g.G(), node) == 0 )
                m_timing.nodes.required( node, m_merge(quantity<si::time>(0.0*seconds), quantity<si::time>(dc.clock.period * pico* seconds)) );
        }

    }


    SlewType rise_arrival(const entity_system::entity pin) const
    {
        return m_timing.nodes.arrival(m_topology->g.rise_node(pin));
    }
    SlewType fall_arrival(const entity_system::entity pin) const
    {
        return m_timing.nodes.arrival(m_topology->g.fall_node(pin));
    }

    SlewType rise_slew(const entity_system::entity pin) const
    {
        return m_timing.nodes.slew(m_topology->g.rise_node(pin));
    }
    SlewType fall_slew(const entity_system::entity pin) const
    {
        return m_timing.nodes.slew(m_topology->g.fall_node(pin));
    }

    SlewType rise_slack(const entity_system::entity pin) const
    {
        auto node = m_topology->g.rise_node(pin);
        return MergeStrategy::slack_signal()*(m_timing.nodes.required(node)-m_timing.nodes.arrival(node));
    }
    SlewType fall_slack(const entity_system::entity pin) const
    {
        auto node = m_topology->g.fall_node(pin);
        return MergeStrategy::slack_signal()*(m_timing.nodes.required(node)-m_timing.nodes.arrival(node));
    }

    void update_ats() {
        {
            for(auto & level : m_topology->levels)
            {
                std::size_t i;
                for(i = 0; i < level.size(); ++i) // paralell
                {
                    auto node = level[i];
                    if(lemon::countInArcs(m_topology->g.G(), node) != 0)
                    {
                        auto pin = m_topology->g.pin(node);
                        auto net = m_topology->netlist.pin_net(pin);
                        auto & tree = m_rc_trees[m_topology->netlist.net_system().lookup(net)];

                        std::vector< SlewType > slews(tree.node_count());
                        std::vector< SlewType > delays(tree.node_count());
                        std::vector< CapacitanceType > ceffs(tree.node_count());
                        WireDelayModel calculator;
                        calculator.delay_map(delays);
                        calculator.slew_map(slews);
                        calculator.ceff_map(ceffs);
                        std::function<SlewType(CapacitanceType)> s_calculator = std::bind(&generic_sta::compute_slew, this, node, std::placeholders::_1);

                        CapacitanceType load = calculator.simulate(s_calculator, tree);

                        m_timing.nodes.load(node, load);
                        m_timing.nodes.slew(node, slews[0]);

                        SlewType worst_arrival = MergeStrategy::best();
                        switch(m_topology->g.node_edge(node))
                        {
                        case edges::RISE:
                            for(lemon::ListDigraph::InArcIt it(m_topology->g.G(), node); it != lemon::INVALID; ++it)
                            {
                                auto tarc = m_topology->g.edge_entity(it) ;
                                auto edge_source = m_topology->g.edge_source(it);
                                auto arc_delay = m_timing.lib.timing_arc_rise_delay(tarc).compute(load, m_timing.nodes.slew(edge_source));
                                auto arc_slew = m_timing.lib.timing_arc_rise_slew(tarc).compute(load, m_timing.nodes.slew(edge_source));
                                m_timing.arcs.delay(it, arc_delay);
                                m_timing.arcs.slew(it, arc_slew);
                                worst_arrival = m_merge(worst_arrival, m_timing.nodes.arrival(edge_source) + arc_delay);
                            }
                            break;
                        case edges::FALL:
                            for(lemon::ListDigraph::InArcIt it(m_topology->g.G(), node); it != lemon::INVALID; ++it)
                            {
                                auto tarc = m_topology->g.edge_entity(it) ;
                                auto edge_source = m_topology->g.edge_source(it);
                                auto arc_delay = m_timing.lib.timing_arc_fall_delay(tarc).compute(load, m_timing.nodes.slew(edge_source));
                                auto arc_slew = m_timing.lib.timing_arc_fall_slew(tarc).compute(load, m_timing.nodes.slew(edge_source));
                                m_timing.arcs.delay(it, arc_delay);
                                m_timing.arcs.slew(it, arc_slew);
                                worst_arrival = m_merge(worst_arrival, m_timing.nodes.arrival(edge_source) + arc_delay);
                            }
                            break;
                        }
                        m_timing.nodes.arrival(node, worst_arrival);
                        for(lemon::ListDigraph::OutArcIt arc(m_topology->g.G(), node); arc != lemon::INVALID; ++arc)
                        {
                            auto arc_target = m_topology->g.edge_target(arc);
                            auto target_pin = m_topology->g.pin(arc_target);
                            auto target_capacitor = tree.tap(m_topology->netlist.pin_name(target_pin));
                            m_timing.arcs.slew(arc, slews[target_capacitor]);
                            m_timing.arcs.delay(arc, delays[target_capacitor]);
                            m_timing.nodes.slew(arc_target, m_timing.arcs.slew(arc));
                            m_timing.nodes.arrival(arc_target, m_timing.nodes.arrival(node) + m_timing.arcs.delay(arc));
                        }
                    }
                }
            }

        }
    }

    void update_rts() {
        for(auto node_it = m_topology->sorted.rbegin(); node_it != m_topology->sorted.rend(); ++node_it)
        {
            auto node = *node_it;
            if(lemon::countOutArcs(m_topology->g.G(), node) > 0)
            {
                SlewType required = MergeStrategy::worst();
                for(lemon::ListDigraph::OutArcIt arc(m_topology->g.G(), node); arc != lemon::INVALID; ++arc)
                    required = m_merge.inverted(required, m_timing.nodes.required(m_topology->g.edge_target(arc))-m_timing.arcs.delay(arc));
                m_timing.nodes.required(node, required);
            }
        }
    }


};

}
}


#endif // OPHIDIAN_TIMING_GENERIC_STA_H

