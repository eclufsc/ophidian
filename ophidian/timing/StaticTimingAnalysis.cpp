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

#include <ophidian/timing/StaticTimingAnalysis.h>

namespace ophidian
{
namespace timing
{

void StaticTimingAnalysis::graph(const timing_graph_type& g)
{
    m_timing_graph = &g;
}

void StaticTimingAnalysis::rc_trees(net_to_rctrees_property_type& trees)
{
    m_rc_trees = &trees;
}

void StaticTimingAnalysis::late_lib(const timing_library_type& lib)
{
    m_late_lib = &lib;
}

void StaticTimingAnalysis::early_lib(const timing_library_type& lib)
{
    m_early_lib = &lib;
}

void StaticTimingAnalysis::netlist(netlist_type & netlist)
{
    m_netlist = &netlist;
}

void StaticTimingAnalysis::lib_mapping(library_mapping_type & lib_mapping)
{
    m_lib_mappping = &lib_mapping;
}

void StaticTimingAnalysis::std_cells(standard_cells_type & std_cells)
{
    m_std_cells = &std_cells;
}

void StaticTimingAnalysis::constraints(const design_constraints_type & dc)
{
    m_dc = dc;
}

void StaticTimingAnalysis::update_timing()
{
    if(!has_timing_data())
        init_timing_data();

    propagate_ats();

    //m_test->compute_tests();

    propagate_rts();

    update_wns_and_tns();
}

void StaticTimingAnalysis::init_timing_data()
{
    m_late_data.reset(new TimingData(*m_late_lib, *m_timing_graph));
    m_early_data.reset(new TimingData(*m_early_lib, *m_timing_graph));
    m_topology.reset(new GraphAndTopology(*m_timing_graph, *m_netlist, *m_std_cells, *m_lib_mappping));
    m_late_sta.reset(new GenericSTA<wiremodel::LumpedCapacitance, Pessimistic>(*m_late_data, *m_topology, *m_rc_trees));
    m_early_sta.reset(new GenericSTA<wiremodel::LumpedCapacitance, Optimistic>(*m_early_data, *m_topology, *m_rc_trees));
    m_endpoints = endpoints_type(*m_netlist, *m_lib_mappping, *m_std_cells, *m_late_lib);
}

void StaticTimingAnalysis::propagate_ats()
{
    m_late_sta->update_ats();
    m_early_sta->update_ats();
}

void StaticTimingAnalysis::propagate_rts()
{
    m_late_sta->update_rts();
    m_early_sta->update_rts();
}

void StaticTimingAnalysis::update_wns_and_tns()
{
    m_lwns = WorstNegativeSlack(m_endpoints, *m_late_sta).value();
    m_ewns = WorstNegativeSlack(m_endpoints, *m_early_sta).value();
    m_ltns = TotalNegativeSlack(m_endpoints, *m_late_sta).value();
    m_etns = TotalNegativeSlack(m_endpoints, *m_early_sta).value();
}

bool StaticTimingAnalysis::has_timing_data() const
{
    assert(m_rc_trees);
    assert(m_timing_graph);
    assert(m_late_lib && m_early_lib);
    assert(m_netlist);
    return m_late_sta && m_early_sta;
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_wns() const
{
    return m_lwns;
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_wns() const
{
    return m_ewns;
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_tns() const
{
    return m_ltns;
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_tns() const
{
    return m_etns;
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_rise_slack(const pin_entity_type& p) const
{
    return m_early_sta->rise_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_slack(const pin_entity_type& p) const
{
    return m_early_sta->fall_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_slack(const pin_entity_type& p) const
{
    return m_late_sta->rise_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_slack(const pin_entity_type& p) const
{
    return m_late_sta->fall_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_rise_arrival(const pin_entity_type& p) const
{
    return m_early_sta->rise_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_arrival(const pin_entity_type& p) const
{
    return m_early_sta->fall_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_arrival(const pin_entity_type& p) const
{
    return m_late_sta->rise_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_arrival(const pin_entity_type& p) const
{
    return m_late_sta->fall_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_rise_slew(const pin_entity_type& p) const
{
    return m_early_sta->rise_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_slew(const pin_entity_type& p) const
{
    return m_early_sta->fall_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_slew(const pin_entity_type& p) const
{
    return m_late_sta->rise_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_slew(const pin_entity_type& p) const
{
    return m_late_sta->fall_slew(p);
}

const StaticTimingAnalysis::endpoints_type & StaticTimingAnalysis::timing_endpoints() const
{
    return m_endpoints;
}

}   // namespace timing
}   // namespace ophidian
