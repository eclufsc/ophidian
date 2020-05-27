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
            if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT || port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
                auto pad = netlist.add_pad(pin);
                if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT){
                    netlist.set_direction(pad, Netlist::pad_direction_type::INPUT); 
                }else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT){
                    netlist.set_direction(pad, Netlist::pad_direction_type::OUTPUT); 
                }
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
            if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT || port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT) {
                auto pad = netlist.add_pad(pin);
                if(port.direction() == parser::Verilog::Module::Port::Direction::INPUT){
                    netlist.set_direction(pad, Netlist::pad_direction_type::INPUT); 
                }else if(port.direction() == parser::Verilog::Module::Port::Direction::OUTPUT){
                    netlist.set_direction(pad, Netlist::pad_direction_type::OUTPUT); 
                }
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

    void make_netlist(Netlist& netlist, const parser::Def & def, const StandardCells& std_cells) noexcept
    {
        for(const auto& component : def.components())
        {
            auto cell_instance = netlist.add_cell_instance(component.name());

            netlist.connect(cell_instance, std_cells.find_cell(component.macro()));
        }

        for(const auto & pad : def.pads())
        {
            auto name = pad.name();
            auto direction = pad.direction();
            auto pin_instance = netlist.add_pin_instance("PIN:" + name);

            if(direction == parser::Pad::Direction::INPUT){
                auto pad = netlist.add_pad(pin_instance);
                netlist.set_direction(pad, Netlist::pad_direction_type::INPUT);
            }else if(direction == parser::Pad::Direction::OUTPUT){
                auto pad = netlist.add_pad(pin_instance);
                netlist.set_direction(pad, Netlist::pad_direction_type::OUTPUT);
            }else if(direction == parser::Pad::Direction::OUTPUT){
                std::cout << "WARNING: Pin " << name << "without direction"<< std::endl;
            }
        }

        for(const auto& net : def.nets())
        {
            auto net_instance = netlist.add_net(net.name());
            for(const auto& pin : net.pins())
            {
                if(pin.first == "PIN")
                {
                    auto pin_instance = netlist.find_pin_instance(pin.first + ":" + pin.second);
                    netlist.connect(net_instance, pin_instance);
                    continue;
                }

                auto pin_instance = netlist.add_pin_instance(pin.first + ":" + pin.second);

                netlist.connect(net_instance, pin_instance);

                auto cell_instance = netlist.find_cell_instance(pin.first);

                netlist.connect(cell_instance, pin_instance);

                auto cell = netlist.std_cell(cell_instance);

                netlist.connect(pin_instance, std_cells.find_pin(std_cells.name(cell) + ":" + pin.second));
            }
        }
    }

    void make_netlist(Netlist& netlist, const parser::ICCAD2020 & iccad_2020, const StandardCells & std_cells) noexcept {
        netlist.reserve_net(iccad_2020.nets().size());
        netlist.reserve_cell_instance(iccad_2020.components().size());
        for(const auto & component : iccad_2020.components())
        {
            auto cell_instance = netlist.add_cell_instance(component.name());

            netlist.connect(cell_instance, std_cells.find_cell(component.macro()));
        }
        for(const auto & net : iccad_2020.nets())
        {
            auto net_instance = netlist.add_net(net.name());
            for(const auto& pin : net.pins())
            {
                auto pin_instance = netlist.add_pin_instance(pin.first + ":" + pin.second);

                netlist.connect(net_instance, pin_instance);

                auto cell_instance = netlist.find_cell_instance(pin.first);

                netlist.connect(cell_instance, pin_instance);

                auto cell = netlist.std_cell(cell_instance);

                netlist.connect(pin_instance, std_cells.find_pin(std_cells.name(cell) + ":" + pin.second));
            }
        }
    }
}
