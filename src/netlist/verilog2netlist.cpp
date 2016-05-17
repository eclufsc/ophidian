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

#include "verilog2netlist.h"

namespace ophidian {
namespace netlist {

void verilog2netlist(const parsing::verilog &verilog, netlist &netlist)
{
    netlist.cell_preallocate(verilog.cell_count());
    netlist.pin_preallocate(verilog.pin_count());
    netlist.net_preallocate(verilog.net_count());
    netlist.module_name(verilog.design());
    for(auto input : verilog.inputs())
        netlist.connect(netlist.net_insert(input), netlist.PI_insert(input));
    for(auto output : verilog.outputs())
        netlist.connect(netlist.net_insert(output), netlist.PO_insert(output));
    for(auto wire : verilog.wires())
        netlist.net_insert(wire);
    for(auto module : verilog.modules())
    {
        auto cell_entity = netlist.cell_insert(module.name, module.type);
        for(auto pinnet_pair : module.pinnet_pairs)
            netlist.connect(netlist.net_insert(pinnet_pair.second), netlist.pin_insert(cell_entity, pinnet_pair.first));
    }
}

}
}
