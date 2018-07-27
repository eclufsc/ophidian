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

#include "SlackCalculation.h"

namespace ophidian
{
namespace timing
{

EndPoints::EndPoints(const netlist_type & netlist,
          const library_mapping_type & library_mapping,
          standard_cells_type & std_cells,
          const timing_library_type & timing_library) :
    m_netlist(netlist),
    m_library_mapping(library_mapping),
    m_std_cells(std_cells),
    m_timing_library(timing_library)
{
}

void EndPoints::init()
{
    m_endpoints.resize(0);

    for(auto output = m_netlist.begin(circuit::Output()); output != m_netlist.end(circuit::Output()); ++output)
        m_endpoints.push_back(m_netlist.pin(*output));

    for(auto pin = m_netlist.begin(pin_entity_type()); pin != m_netlist.end(pin_entity_type()); ++pin)
    {
        auto pin_owner    = m_netlist.cell(*pin);
        auto pin_std_cell = m_library_mapping.pinStdCell(*pin);

        if(pin_owner == circuit::Cell())
            continue;

        auto owner_std_cell = m_library_mapping.cellStdCell(pin_owner);

        if(!m_timing_library.cell_sequential(owner_std_cell))
            continue;
        else if(m_std_cells.direction(pin_std_cell) != standard_cell::PinDirection::INPUT)
            continue;
        else if(m_timing_library.pin_clock(pin_std_cell))
            continue;

        m_endpoints.push_back(*pin);
    }
}

EndPoints::pin_container_type::const_iterator EndPoints::begin() const
{
    return m_endpoints.cbegin();
}

EndPoints::pin_container_type::const_iterator EndPoints::end() const
{
    return m_endpoints.cend();
}

std::size_t EndPoints::size() const
{
    return m_endpoints.size();
}

bool EndPoints::empty() const
{
    return m_endpoints.empty();
}

const TotalNegativeSlack::time_unit_type TotalNegativeSlack::value() const
{
    return m_value;
}

const WorstNegativeSlack::time_unit_type WorstNegativeSlack::value() const
{
    return m_value;
}

}   // namespace timing
}   // namespace ophidian
