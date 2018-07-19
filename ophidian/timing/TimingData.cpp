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

#include "TimingData.h"

namespace ophidian
{
namespace timing
{
    TimingData::TimingData(const timing_library_type& lib, const timing_graph_type& graph) :
        m_library(lib),
        m_node_arrivals(graph.graph(), time_unit_type(0)),
        m_node_slews(graph.graph(), time_unit_type(0)),
        m_node_requireds(graph.graph(), time_unit_type(0)),
        m_node_loads(graph.graph(), capacitance_unit_type(0)),
        m_arc_delays(graph.graph(), time_unit_type(0)),
        m_arc_slews(graph.graph(), time_unit_type(0))
    {
    }

    const TimingData::timing_library_type& TimingData::library()
    {
        return m_library;
    }

    void TimingData::arrival(node_type node, time_unit_type arrival)
    {
        m_node_arrivals[node] = arrival;
    }

    void TimingData::slew(node_type node, time_unit_type slew)
    {
        m_node_slews[node] = slew;
    }

    void TimingData::required(node_type node, time_unit_type required)
    {
        m_node_requireds[node] = required;
    }

    void TimingData::load(node_type node, capacitance_unit_type load)
    {
        m_node_loads[node] = load;
    }

    const TimingData::time_unit_type TimingData::arrival(node_type node) const
    {
        return m_node_arrivals[node];
    }

    const TimingData::time_unit_type TimingData::slew(node_type node) const
    {
        return m_node_slews[node];
    }

    const TimingData::time_unit_type TimingData::required(node_type node) const
    {
        return m_node_requireds[node];
    }

    const TimingData::capacitance_unit_type TimingData::load(node_type node) const
    {
        return m_node_loads[node];
    }

// ------------------------------------------------------------------------------

    void TimingData::delay(arc_type arc, const time_unit_type delay)
    {
        m_arc_delays[arc] = delay;
    }

    void TimingData::slew(arc_type arc, const time_unit_type slew)
    {
        m_arc_slews[arc] = slew;
    }

    const TimingData::time_unit_type TimingData::delay(arc_type arc) const
    {
        return m_arc_delays[arc];
    }
    const TimingData::time_unit_type TimingData::slew(arc_type arc) const
    {
        return m_arc_slews[arc];
    }

}   // namespace timing
}   // namespace ophidian
