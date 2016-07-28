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

#include "extract_flip_flops.h"

namespace ophidian {
namespace clock_tree_synthesis {

void extract_flip_flops(const netlist::netlist &netlist, const standard_cell::standard_cells &standard_cells, std::vector<entity_system::entity> &flip_flops)
{
    for (auto cell : netlist.cell_system()) {
        auto std_cell = netlist.cell_std_cell(cell);
        if (standard_cells.cell_sequential(std_cell)) {
            flip_flops.push_back(cell);
        }
    }
}

}
}

