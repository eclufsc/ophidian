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
#include <ophidian/timing/TimingGraphBuilder.h>
#include <ophidian/timingdriven_placement/FluteRCTreeEstimation.h>

namespace ophidian
{
namespace timing
{

class StaticTimingAnalysis
{
public:
    template <class T>
    using generic_sta_type             = GenericSTA<wiremodel::LumpedCapacitance, T>;

    using time_unit_type               = util::second_t;
    using timing_data_type             = TimingData;
    using graph_and_topology_type      = GraphAndTopology;

    using timing_graph_type            = TimingGraph;
    using timing_arcs_type             = TimingArcs;
    using rc_tree_type                 = timingdriven_placement::RCTree;
    using net_to_rctrees_property_type = entity_system::Property<circuit::Net, timingdriven_placement::RCTree>;
    using timing_library_type          = Library;
    using placement_type               = placement::Placement;
    using placment_library_type        = placement::PlacementMapping;
    using netlist_type                 = circuit::Netlist;
    using pin_entity_type              = circuit::Pin;
    using library_mapping_type         = circuit::LibraryMapping;
    using standard_cells_type          = standard_cell::StandardCells;
    using design_constraints_type      = parser::DesignConstraints;
    using lef_type                     = parser::Lef;
    using liberty_type                 = parser::Liberty;
    using endpoints_type               = EndPoints;

    //! Deleted Default Constructor
    StaticTimingAnalysis() = delete;

    //! Copy Constructor
    StaticTimingAnalysis(const StaticTimingAnalysis&) = delete;
    StaticTimingAnalysis& operator=(const StaticTimingAnalysis&) = delete;

    //! Move Constructor
    StaticTimingAnalysis(StaticTimingAnalysis&&) = default;
    StaticTimingAnalysis& operator=(StaticTimingAnalysis&&) = default;

    //! StaticTimingAnalysis Constructor
    StaticTimingAnalysis(standard_cells_type& std_cells,
                         netlist_type& netlist,
                         library_mapping_type& lib_mapping,
                         placement_type& placement,
                         placment_library_type& placement_mapping);

    void init(const liberty_type& early, const liberty_type& late, const lef_type& lef, const design_constraints_type& dc);

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
    void propagate_ats();
    void propagate_rts();
    void update_wns_and_tns();

    placement_type&               m_placement;
    placment_library_type&        m_placement_mapping;
    timing_arcs_type              m_timing_arcs;
    timing_library_type           m_early_lib;
    timing_library_type           m_late_lib;
    timing_graph_type             m_timing_graph;
    timing_data_type              m_early_data;
    timing_data_type              m_late_data;
    graph_and_topology_type       m_topology;
    net_to_rctrees_property_type  m_rc_trees;
    generic_sta_type<Optimistic>  m_early_sta;
    generic_sta_type<Pessimistic> m_late_sta;
    endpoints_type                m_endpoints;
    time_unit_type                m_lwns;
    time_unit_type                m_ewns;
    time_unit_type                m_ltns;
    time_unit_type                m_etns;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_OPHIDIAN_TIMING_ELMORE_H

