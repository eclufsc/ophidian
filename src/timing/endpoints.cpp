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

#include "endpoints.h"

namespace ophidian {
namespace timing {

endpoints::endpoints() {

}

endpoints::endpoints(const netlist::netlist & netlist) : m_entities(netlist.PO_count())
{
    m_entities.resize(0);
    for(auto PO = netlist.PO_begin(); PO != netlist.PO_end(); ++PO)
        m_entities.push_back(*PO);
    for(auto pin : netlist.pin_system())
    {
        auto pin_owner = netlist.pin_owner(pin.first);
        auto pin_std_cell = netlist.pin_std_cell(pin.first);
        if(pin_owner == entity::entity{}) continue;
        auto owner_std_cell = netlist.cell_std_cell(pin_owner);
        if(!netlist.std_cells().cell_sequential(owner_std_cell)) continue;
        if(netlist.std_cells().pin_direction(pin_std_cell) != standard_cell::pin_directions::INPUT) continue;
        if(netlist.std_cells().pin_clock_input(pin_std_cell)) continue;
        m_entities.push_back(pin.first);
    }
}

endpoints::~endpoints()
{

}

}
}
