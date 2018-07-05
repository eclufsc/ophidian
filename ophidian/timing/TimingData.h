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

#ifndef OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H
#define OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H

#include <ophidian/timing/TimingGraph.h>

namespace ophidian
{
namespace timing
{

class TimingData
{
public:
    using time_unit_type               = util::second_t;
    using capacitance_unit_type        = util::farad_t;

    using timing_library_type          = Library;
    using timing_graph_type            = TimingGraph;
    using node_type                    = timing_graph_type::node_type;
    using arc_type                     = timing_graph_type::arc_type;
    using node_to_time_map_type        = timing_graph_type::node_map_type<time_unit_type>;
    using node_to_capacitance_map_type = timing_graph_type::node_map_type<capacitance_unit_type>;
    using arc_to_time_map_type         = timing_graph_type::arc_map_type<time_unit_type>;

    TimingData() = delete;

    TimingData(const TimingData&) = delete;
    TimingData& operator=(const TimingData&) = delete;

    TimingData(TimingData&&) = default;
    TimingData& operator=(TimingData&&) = default;

    TimingData(const timing_library_type& lib, const timing_graph_type& graph);

    ~TimingData() = default;

    const timing_library_type& library();

    /* ------------------   nodes   ------------------ */

    void arrival(node_type node, time_unit_type arrival);
    void slew(node_type node, time_unit_type slew);
    void required(node_type node, time_unit_type required);
    void load(node_type node, capacitance_unit_type load);

    const time_unit_type arrival(node_type node) const;
    const time_unit_type slew(node_type node) const;
    const time_unit_type required(node_type node) const;
    const capacitance_unit_type load(node_type node) const;

    /* ------------------   arcs   ------------------ */

    void delay(arc_type arc, const time_unit_type delay);
    void slew(arc_type arc, const time_unit_type slew);

    const time_unit_type delay(arc_type arc) const;
    const time_unit_type slew(arc_type arc) const;

private:
    const timing_library_type&   m_library;
    node_to_time_map_type        m_node_arrivals;
    node_to_time_map_type        m_node_slews;
    node_to_time_map_type        m_node_requireds;
    node_to_capacitance_map_type m_node_loads;
    arc_to_time_map_type         m_arc_delays;
    arc_to_time_map_type         m_arc_slews;
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_TIMINGGRAPHCONDENSATION_H
