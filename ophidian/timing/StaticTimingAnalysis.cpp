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

StaticTimingAnalysis::StaticTimingAnalysis(standard_cells_type& std_cells,
                                           netlist_type& netlist,
                                           library_mapping_type& lib_mapping,
                                           placement_type& placement,
                                           placment_library_type& placement_mapping) :
    m_placement(placement),
    m_placement_mapping(placement_mapping),
    m_timing_arcs(std_cells),
    m_early_lib(std_cells, m_timing_arcs),
    m_late_lib(std_cells, m_timing_arcs),
    m_timing_graph(netlist),
    m_early_data(m_early_lib, m_timing_graph),
    m_late_data(m_late_lib, m_timing_graph),
    m_topology(m_timing_graph, netlist, std_cells, lib_mapping),
    m_rc_trees(netlist.makeProperty<rc_tree_type>(circuit::Net())),
    m_early_sta(m_early_data, m_topology, m_rc_trees),
    m_late_sta(m_late_data, m_topology, m_rc_trees),
    m_endpoints(netlist, lib_mapping, std_cells, m_early_lib)
{

}

void StaticTimingAnalysis::init(const liberty_type& early, const liberty_type& late, const lef_type& lef, const design_constraints_type& dc)
{
    m_early_lib.init(early, true);
    m_late_lib.init(late, false);
    TimingGraphBuilder().build(m_topology.m_netlist, m_topology.m_std_cells, m_topology.m_library_mapping, m_timing_arcs, m_early_lib, dc, m_timing_graph);
    m_topology.init();
    m_late_sta.init(dc);
    m_early_sta.init(dc);
    m_endpoints.init();

    timingdriven_placement::FluteRCTreeBuilder builder;

    for (auto it = m_topology.m_netlist.begin(circuit::Net()); it != m_topology.m_netlist.end(circuit::Net()); ++it)
    {
        circuit::Pin source;

        for (auto pin : m_topology.m_netlist.pins(*it))
            if (m_topology.m_std_cells.direction(m_topology.m_library_mapping.pinStdCell(pin)) == standard_cell::PinDirection::OUTPUT)
            {
                source = pin;
                break;
            }

        builder.build(m_placement, m_placement_mapping, m_topology.m_library_mapping, m_topology.m_netlist, m_early_lib, lef, *it, m_rc_trees[*it], source);
    }

    update_timing();
}

void StaticTimingAnalysis::update_timing()
{
    propagate_ats();
    propagate_rts();
    update_wns_and_tns();
}

void StaticTimingAnalysis::propagate_ats()
{
    m_late_sta.update_ats();
    m_early_sta.update_ats();
}

void StaticTimingAnalysis::propagate_rts()
{
    m_late_sta.update_rts();
    m_early_sta.update_rts();
}

void StaticTimingAnalysis::update_wns_and_tns()
{
    m_lwns = WorstNegativeSlack(m_endpoints, m_late_sta).value();
    m_ewns = WorstNegativeSlack(m_endpoints, m_early_sta).value();
    m_ltns = TotalNegativeSlack(m_endpoints, m_late_sta).value();
    m_etns = TotalNegativeSlack(m_endpoints, m_early_sta).value();
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
    return m_early_sta.rise_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_slack(const pin_entity_type& p) const
{
    return m_early_sta.fall_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_slack(const pin_entity_type& p) const
{
    return m_late_sta.rise_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_slack(const pin_entity_type& p) const
{
    return m_late_sta.fall_slack(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_rise_arrival(const pin_entity_type& p) const
{
    return m_early_sta.rise_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_arrival(const pin_entity_type& p) const
{
    return m_early_sta.fall_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_arrival(const pin_entity_type& p) const
{
    return m_late_sta.rise_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_arrival(const pin_entity_type& p) const
{
    return m_late_sta.fall_arrival(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_rise_slew(const pin_entity_type& p) const
{
    return m_early_sta.rise_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::early_fall_slew(const pin_entity_type& p) const
{
    return m_early_sta.fall_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_rise_slew(const pin_entity_type& p) const
{
    return m_late_sta.rise_slew(p);
}

StaticTimingAnalysis::time_unit_type StaticTimingAnalysis::late_fall_slew(const pin_entity_type& p) const
{
    return m_late_sta.fall_slew(p);
}

const StaticTimingAnalysis::endpoints_type & StaticTimingAnalysis::timing_endpoints() const
{
    return m_endpoints;
}

}   // namespace timing
}   // namespace ophidian
