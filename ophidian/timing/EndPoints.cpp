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

#include "EndPoints.h"

namespace ophidian
{
namespace timing
{

EndPoints::EndPoints(const circuit::Netlist & netlist,
          const circuit::LibraryMapping & lib_mapping,
          standard_cell::StandardCells & std_cells,
          const Library & lib)
{
    m_endpoints.resize(0);

    for(auto output = netlist.begin(circuit::Output()); output != netlist.end(circuit::Output()); ++output)
        m_endpoints.push_back(netlist.pin(*output));

    for(auto pin = netlist.begin(circuit::Pin()); pin != netlist.end(circuit::Pin()); ++pin)
    {
        auto pin_owner    = netlist.cell(*pin);
        auto pin_std_cell = lib_mapping.pinStdCell(*pin);

        if(pin_owner == circuit::Cell())
            continue;

        auto owner_std_cell = lib_mapping.cellStdCell(pin_owner);

        if(!lib.cellSequential(owner_std_cell))
            continue;
        else if(std_cells.direction(pin_std_cell) != standard_cell::PinDirection::INPUT)
            continue;
        else if(lib.pinClock(pin_std_cell))
            continue;

        m_endpoints.push_back(*pin);
    }
}

EndPoints::~EndPoints()
{

}

EndPoints::container_type::const_iterator EndPoints::begin() const
{
    return m_endpoints.begin();
}

EndPoints::container_type::const_iterator EndPoints::end() const
{
    return m_endpoints.end();
}

std::size_t EndPoints::size() const
{
    return m_endpoints.size();
}

bool EndPoints::empty() const
{
    return m_endpoints.empty();
}

}   // namespace timing
}   // namespace ophidian
