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

#include "clock_tree_synthesis.h"

namespace ophidian {
namespace clock_tree_synthesis {
clock_tree_synthesis::clock_tree_synthesis()
    : m_flip_flops(m_flip_flops_system) {

}

clock_tree_synthesis::~clock_tree_synthesis()
{

}

entity_system::entity clock_tree_synthesis::flip_flop_insert(entity_system::entity netlist_cell)
{
    auto flip_flop = m_flip_flops_system.create();
    m_flip_flops.netlist_cell(flip_flop, netlist_cell);
    return flip_flop;
}

void clock_tree_synthesis::flip_flop_remove(entity_system::entity flip_flop)
{
    m_flip_flops_system.destroy(flip_flop);
}

}
}

