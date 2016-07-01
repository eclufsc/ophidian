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

#include "flip_flops.h"

namespace ophidian {
namespace clock_tree_synthesis {
flip_flops::flip_flops(entity_system::entity_system &system)
    : m_system(system){
    m_system.register_property(&m_netlist_cells);
}

flip_flops::~flip_flops()
{

}

void flip_flops::netlist_cell(entity_system::entity flip_flop, entity_system::entity netlist_cell)
{
    m_netlist_cells[m_system.lookup(flip_flop)] = netlist_cell;
}
}
}

