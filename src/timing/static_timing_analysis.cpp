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

#include "static_timing_analysis.h"
#include "graph_builder.h"

#include "wns.h"

namespace ophidian {
namespace timing {

void static_timing_analysis::init_timing_data()
{
    m_late.reset(new timing::timing_data(*m_late_lib, *m_timing_graph));
    m_early.reset(new timing::timing_data(*m_early_lib, *m_timing_graph));
    m_topology.reset(new timing::graph_and_topology(*m_timing_graph, *m_netlist, *m_late_lib));
    m_late_sta.reset(new timing::generic_sta<timing::effective_capacitance_wire_model, timing::pessimistic>(*m_late, *m_topology, *m_rc_trees));
    m_early_sta.reset(new timing::generic_sta<timing::effective_capacitance_wire_model, timing::optimistic>(*m_early, *m_topology, *m_rc_trees));
    m_test.reset(new timing::test_calculator{*m_topology, *m_early, *m_late, TimeType(m_dc.clock.period*boost::units::si::pico*boost::units::si::seconds)});
    m_endpoints = timing::endpoints(*m_netlist);
}

void static_timing_analysis::propagate_ats()
{
    m_late_sta->update_ats();
    m_early_sta->update_ats();
}

void static_timing_analysis::propagate_rts()
{
    m_late_sta->update_rts();
    m_early_sta->update_rts();
}

void static_timing_analysis::update_wns_and_tns()
{
    m_lwns = timing::wns(m_endpoints, *m_late_sta).value();
    m_ewns = timing::wns(m_endpoints, *m_early_sta).value();
}

static_timing_analysis::static_timing_analysis() :
    m_timing_graph(nullptr),
    m_rc_trees(nullptr)
{

}

void static_timing_analysis::update_timing()
{
    if(!has_timing_data())
        init_timing_data();

    propagate_ats();

    m_test->compute_tests();

    propagate_rts();

    update_wns_and_tns();
}


void static_timing_analysis::graph(const ophidian::timing::graph &g)
{
    m_timing_graph = &g;
}

void static_timing_analysis::rc_trees(const entity_system::vector_property<interconnection::packed_rc_tree> &trees)
{
    m_rc_trees = &trees;
}

void static_timing_analysis::late_lib(const library &lib)
{
    m_late_lib = &lib;
}

void static_timing_analysis::early_lib(const library &lib)
{
    m_early_lib = &lib;
}

void static_timing_analysis::netlist(const netlist::netlist &netlist)
{
    m_netlist = &netlist;
}

void static_timing_analysis::set_constraints(const design_constraints &dc)
{
    m_dc = dc;
}

}
}
