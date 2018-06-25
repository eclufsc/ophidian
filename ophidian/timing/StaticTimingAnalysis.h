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

#ifndef OPHIDIAN_TIMING_STATICTIMINGANALYSISS_H
#define OPHIDIAN_TIMING_STATICTIMINGANALYSISS_H

#include <ophidian/util/Units.h>
#include <ophidian/parser/SDCParser.h>

#include <ophidian/timing/Library.h>
#include <ophidian/timing/TimingGraph.h>
#include <ophidian/timing/GenericSTA.h>
#include <ophidian/timing/WireModels.h>
#include <ophidian/timing/SlackCalculation.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>

namespace ophidian
{
namespace timing
{

class StaticTimingAnalysis
{
public:
    template <class T>
    using generic_sta_lumped_ptr       = std::unique_ptr<GenericSTA<wiremodel::LumpedCapacitance, T>>;
    
    using time_unit_type               = util::second_t;
    using timing_data_ptr              = std::unique_ptr<TimingData>;
    using graph_and_topology_ptr       = std::unique_ptr<GraphAndTopology>;

    using timing_graph_type            = TimingGraph;
    using net_to_rctrees_property_type = entity_system::Property<circuit::Net, timingdriven_placement::RCTree>;
    using timing_library_type          = Library;
    using netlist_type                 = circuit::Netlist;
    using pin_entity_type              = circuit::Pin;
    using library_mapping_type         = circuit::LibraryMapping;
    using standard_cells_type          = standard_cell::StandardCells;
    using design_constraints_type      = parser::DesignConstraints;
    using endpoints_type               = EndPoints;

    StaticTimingAnalysis() = default;

    StaticTimingAnalysis(const StaticTimingAnalysis&) = delete;
    StaticTimingAnalysis& operator=(const StaticTimingAnalysis&) = delete;

    StaticTimingAnalysis(StaticTimingAnalysis&&) = default;
    StaticTimingAnalysis& operator=(StaticTimingAnalysis&&) = default;

    void graph(const timing_graph_type& g);
    void rc_trees(net_to_rctrees_property_type& trees);
    void late_lib(const timing_library_type& lib);
    void early_lib(const timing_library_type& lib);
    void netlist(netlist_type & netlist);
    void lib_mapping(library_mapping_type & lib_mapping);
    void std_cells(standard_cells_type & std_cells);
    void constraints(const design_constraints_type & dc);

    void update_timing();

    time_unit_type late_wns() const;
    time_unit_type early_wns() const;
    time_unit_type late_tns() const;
    time_unit_type early_tns() const;
    time_unit_type early_rise_slack(const pin_entity_type& p) const;
    time_unit_type early_fall_slack(const pin_entity_type& p) const;
    time_unit_type late_rise_slack(const pin_entity_type& p) const;
    time_unit_type late_fall_slack(const pin_entity_type& p) const;
    time_unit_type early_rise_arrival(const pin_entity_type& p) const;
    time_unit_type early_fall_arrival(const pin_entity_type& p) const;
    time_unit_type late_rise_arrival(const pin_entity_type& p) const;
    time_unit_type late_fall_arrival(const pin_entity_type& p) const;
    time_unit_type early_rise_slew(const pin_entity_type& p) const;
    time_unit_type early_fall_slew(const pin_entity_type& p) const;
    time_unit_type late_rise_slew(const pin_entity_type& p) const;
    time_unit_type late_fall_slew(const pin_entity_type& p) const;

    const endpoints_type & timing_endpoints() const;

private:
    void init_timing_data();
    void propagate_ats();
    void propagate_rts();
    void update_wns_and_tns();
    bool has_timing_data() const;

    const timing_graph_type*            m_timing_graph;
    net_to_rctrees_property_type*       m_rc_trees;
    const timing_library_type*          m_late_lib;
    const timing_library_type*          m_early_lib;
    netlist_type*                       m_netlist;
    library_mapping_type*               m_lib_mappping;
    standard_cells_type*                m_std_cells;
    design_constraints_type             m_dc;

    timing_data_ptr                     m_late_data;
    timing_data_ptr                     m_early_data;
    graph_and_topology_ptr              m_topology;
    generic_sta_lumped_ptr<Pessimistic> m_late_sta;
    generic_sta_lumped_ptr<Optimistic>  m_early_sta;
    endpoints_type                      m_endpoints;
    time_unit_type                      m_lwns;
    time_unit_type                      m_ewns;
    time_unit_type                      m_ltns;
    time_unit_type                      m_etns;

    // lazy pointers
    //std::unique_ptr<test_calculator> m_test;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_OPHIDIAN_TIMING_ELMORE_H

