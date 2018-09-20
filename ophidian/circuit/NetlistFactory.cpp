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

namespace ophidian::circuit::factory
{
    void make_netlist(Netlist& netlist, const parser::Verilog & verilog) noexcept
    {
        const parser::Verilog::Module & module = verilog.modules().front();

        std::size_t sizePins = 0;
        for(auto instance : module.module_instances())
        {
            sizePins += instance.net_map().size();
        }
        sizePins += module.ports().size();

        netlist.reserve_pin_instance(sizePins);
        netlist.reserve_net(module.nets().size());
        netlist.reserve_cell_instance(module.module_instances().size());

        for(auto net : module.nets())
        {
            netlist.add_net(net.name());
        }

        for(auto port : module.ports())
        {
            auto pin = netlist.add_pin_instance(port.name());
            if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT) {
                netlist.add_input_pad(pin);
            }
            else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
                netlist.add_output_pad(pin);
            }
            netlist.connect(netlist.find_net(port.name()), pin);
        }

        for(auto instance : module.module_instances())
        {
            auto cell = netlist.add_cell_instance(instance.name());
            for(auto portMap : instance.net_map())
            {
                auto pin = netlist.add_pin_instance(instance.name() + ":" + portMap.first);
                netlist.connect(cell, pin);
                netlist.connect(netlist.find_net(portMap.second), pin);
            }
        }
    }

    void make_netlist(Netlist& netlist, const parser::Verilog & verilog, const StandardCells& std_cells) noexcept
    {
        const auto& module = verilog.modules().front();

        std::size_t sizePins = 0;
        for(auto& instance : module.module_instances())
        {
            sizePins += instance.net_map().size();
        }
        sizePins += module.ports().size();

        netlist.reserve_pin_instance(sizePins);
        netlist.reserve_net(module.nets().size());
        netlist.reserve_cell_instance(module.module_instances().size());

        for(auto& net : module.nets())
        {
            netlist.add_net(net.name());
        }

        for(auto& port : module.ports())
        {
            auto pin = netlist.add_pin_instance(port.name());
            if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT) {
                netlist.add_input_pad(pin);
            }
            else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
                netlist.add_output_pad(pin);
            }
            netlist.connect(netlist.find_net(port.name()), pin);
        }

        for(auto& instance : module.module_instances())
        {
            auto cell = netlist.add_cell_instance(instance.name());

            netlist.connect(cell, std_cells.find_cell(instance.module()));
            for(auto portMap : instance.net_map())
            {
                auto pin = netlist.add_pin_instance(instance.name() + ":" + portMap.first);
                netlist.connect(cell, pin);
                
                netlist.connect(pin, std_cells.find_pin(instance.module() + ":" + portMap.first));

                netlist.connect(netlist.find_net(portMap.second), pin);
            }
        }
    }


    void make_netlist(Netlist &netlist, const parser::Def &def, const StandardCells &std_cells) noexcept
    {
//        const auto& module = verilog.modules().front();

//        std::size_t sizePins = 0;
//        for(auto& instance : module.module_instances())
//        {
//            sizePins += instance.net_map().size();
//        }
//        sizePins += module.ports().size();

//        netlist.reserve_pin_instance(sizePins);
//        netlist.reserve_net(module.nets().size());
//        netlist.reserve_cell_instance(module.module_instances().size());

//        for(auto& net : module.nets())
//        {
//            netlist.add_net(net.name());
//        }

//        for(auto& port : module.ports())
//        {
//            auto pin = netlist.add_pin_instance(port.name());
//            if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT) {
//                netlist.add_input_pad(pin);
//            }
//            else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
//                netlist.add_output_pad(pin);
//            }
//            netlist.connect(netlist.find_net(port.name()), pin);
//        }

//        for(auto& instance : module.module_instances())
//        {
//            auto cell = netlist.add_cell_instance(instance.name());

//            netlist.connect(cell, std_cells.find_cell(instance.module()));
//            for(auto portMap : instance.net_map())
//            {
//                auto pin = netlist.add_pin_instance(instance.name() + ":" + portMap.first);
//                netlist.connect(cell, pin);

//                netlist.connect(pin, std_cells.find_pin(instance.module() + ":" + portMap.first));

//                netlist.connect(netlist.find_net(portMap.second), pin);
//            }
//        }
    }

}
