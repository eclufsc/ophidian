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

#include "NetlistFactory.h"

namespace ophidian
{
namespace circuit
{
    namespace factory
    {
        Netlist make_netlist(const parser::Verilog & verilog) noexcept
        {
            auto netlist = Netlist{};

            const parser::Verilog::Module & module = verilog.modules().front();

            std::size_t sizePins = 0;
            for(auto instance : module.module_instances())
            {
                sizePins += instance.net_map().size();
            }
            sizePins += module.ports().size();

            netlist.reserve(PinInstance(), sizePins);
            netlist.reserve(Net(), module.nets().size());
            netlist.reserve(CellInstance(), module.module_instances().size());


            for(auto net : module.nets())
            {
                netlist.add(Net(), net.name());
            }

            for(auto port : module.ports())
            {
                auto pin = netlist.add(PinInstance(), port.name());
                if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT) {
                    netlist.add(Input(), pin);
                }
                else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
                    netlist.add(Output(), pin);
                }
                netlist.connect(netlist.find(Net(), port.name()), pin);
            }

            for(auto instance : module.module_instances())
            {
                auto cell = netlist.add(CellInstance(), instance.name());
                for(auto portMap : instance.net_map())
                {
                    auto pin = netlist.add(PinInstance(), instance.name() + ":" + portMap.first);
                    netlist.add(cell, pin);
                    netlist.connect(netlist.find(Net(), portMap.second), pin);
                }
            }

            return netlist;
        }
    }
}     // namespace circuit
}     // namespace ophidian
