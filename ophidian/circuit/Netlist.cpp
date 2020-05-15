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

#include "Netlist.h"

namespace ophidian::circuit
{
    Netlist::cell_instance_type Netlist::find_cell_instance(const Netlist::cell_instance_name_type& cellName) const
    {
        return m_name_to_cell.at(cellName);
    }

    Netlist::pin_instance_type Netlist::find_pin_instance(const Netlist::pin_instance_name_type& pinName) const
    {
        return m_name_to_pin.at(pinName);
    }

    Netlist::net_type Netlist::find_net(const Netlist::net_name_type& netName) const
    {
        return m_name_to_net.at(netName);
    }

    Netlist::cell_instance_name_type& Netlist::name(const Netlist::cell_instance_type& cell)
    {
        return m_cell_names[cell];
    }

    const Netlist::cell_instance_name_type& Netlist::name(const Netlist::cell_instance_type& cell) const
    {
        return m_cell_names[cell];
    }

    Netlist::pin_instance_name_type& Netlist::name(const Netlist::pin_instance_type& pin)
    {
        return m_pin_names[pin];
    }

    const Netlist::pin_instance_name_type& Netlist::name(const Netlist::pin_instance_type& pin) const
    {
        return m_pin_names[pin];
    }

    Netlist::net_name_type& Netlist::name(const Netlist::net_type& net)
    {
        return m_net_names[net];
    }

    const Netlist::net_name_type& Netlist::name(const Netlist::net_type& net) const
    {
        return m_net_names[net];
    }

    Netlist::cell_instance_type Netlist::cell(const Netlist::pin_instance_type& pin) const
    {
        return m_cell_to_pins.whole(pin);
    }

    Netlist::cell_instance_pins_view_type Netlist::pins(const Netlist::cell_instance_type& cell) const
    {
        return m_cell_to_pins.parts(cell);
    }

    Netlist::net_pins_view_type Netlist::pins(const Netlist::net_type& net) const
    {
        return m_net_to_pins.parts(net);
    }

    Netlist::pin_instance_type Netlist::pin(const Netlist::input_pad_type& input) const
    {
        return m_pin_to_input_pad.whole(input);
    }

    Netlist::pin_instance_type Netlist::pin(const Netlist::output_pad_type& output) const
    {
        return m_pin_to_output_pad.whole(output);
    }

    Netlist::net_type Netlist::net(const Netlist::pin_instance_type& pin) const
    {
        return m_net_to_pins.whole(pin);
    }

    Netlist::input_pad_type Netlist::input(const Netlist::pin_instance_type& pin) const
    {
        return m_pin_to_input_pad.firstPart(pin);
    }

    Netlist::output_pad_type Netlist::output(const Netlist::pin_instance_type& pin) const
    {
        return m_pin_to_output_pad.firstPart(pin);
    }

    Netlist::std_cell_type Netlist::std_cell(const Netlist::cell_instance_type& cell) const
    {
        return m_cell_instance_to_std_cell[cell];
    }

    Netlist::std_cell_pin_type Netlist::std_cell_pin(const Netlist::pin_instance_type& pin) const
    {
        return m_pin_instance_to_std_cell_pin[pin];
    }

    bool Netlist::is_pad(const Netlist::pin_instance_type& pin) const
    {
        auto name = m_pin_names[pin];
        if (name.find("PIN:") != std::string::npos) {
            //.. found.
            return true;
        } 
        return false;
    }

    Netlist::cell_instance_container_type::const_iterator Netlist::begin_cell_instance() const noexcept
    {
        return m_cells.begin();
    }

    Netlist::cell_instance_container_type::const_iterator Netlist::end_cell_instance() const noexcept
    {
        return m_cells.end();
    }

    Netlist::pin_instance_container_type::const_iterator Netlist::begin_pin_instance() const noexcept
    {
        return m_pins.begin();
    }

    Netlist::pin_instance_container_type::const_iterator Netlist::end_pin_instance() const noexcept
    {
        return m_pins.end();
    }

    Netlist::net_container_type::const_iterator Netlist::begin_net() const noexcept
    {
        return m_nets.begin();
    }

    Netlist::net_container_type::const_iterator Netlist::end_net() const noexcept
    {
        return m_nets.end();
    }

    Netlist::input_pad_container_type::const_iterator Netlist::begin_input_pad() const noexcept
    {
        return m_input_pads.begin();
    }

    Netlist::input_pad_container_type::const_iterator Netlist::end_input_pad() const noexcept
    {
        return m_input_pads.end();
    }

    Netlist::output_pad_container_type::const_iterator Netlist::begin_output_pad() const noexcept
    {
        return m_output_pads.begin();
    }

    Netlist::output_pad_container_type::const_iterator Netlist::end_output_pad() const noexcept
    {
        return m_output_pads.end();
    }

    Netlist::cell_instance_container_type::size_type Netlist::size_cell_instance() const noexcept
    {
        return m_cells.size();
    }

    Netlist::pin_instance_container_type::size_type Netlist::size_pin_instance() const noexcept
    {
        return m_pins.size();
    }

    Netlist::net_container_type::size_type Netlist::size_net() const noexcept
    {
        return m_nets.size();
    }

    Netlist::input_pad_container_type::size_type Netlist::size_input_pad() const noexcept
    {
        return m_input_pads.size();
    }

    Netlist::output_pad_container_type::size_type Netlist::size_output_pad() const noexcept
    {
        return m_output_pads.size();
    }

    Netlist::cell_instance_container_type::size_type Netlist::capacity_cell_instance() const noexcept
    {
        return m_cells.capacity();
    }

    Netlist::pin_instance_container_type::size_type Netlist::capacity_pin_instance() const noexcept
    {
        return m_pins.capacity();
    }

    Netlist::net_container_type::size_type Netlist::capacity_net() const noexcept
    {
        return m_nets.capacity();
    }

    void Netlist::reserve_cell_instance(Netlist::cell_instance_container_type::size_type size)
    {
        m_cells.reserve(size);
        m_name_to_cell.reserve(size);
    }

    void Netlist::reserve_pin_instance(Netlist::pin_instance_container_type::size_type size)
    {
        m_pins.reserve(size);
        m_name_to_pin.reserve(size);
    }

    void Netlist::reserve_net(Netlist::net_container_type::size_type size)
    {
        m_nets.reserve(size);
        m_name_to_net.reserve(size);
    }

    void Netlist::shrink_to_fit()
    {
        m_cells.shrinkToFit();
        m_pins.shrinkToFit();
        m_nets.shrinkToFit();
        m_input_pads.shrinkToFit();
        m_output_pads.shrinkToFit();
    }

    Netlist::cell_instance_type Netlist::add_cell_instance(const Netlist::cell_instance_name_type& cellName)
    {
        if(m_name_to_cell.find(cellName) == m_name_to_cell.end()) {
            auto cell = m_cells.add();
            m_cell_names[cell] = cellName;
            m_name_to_cell[cellName] = cell;

            return cell;
        }
        else {
            return m_name_to_cell[cellName];
        }
    }

    Netlist::pin_instance_type Netlist::add_pin_instance(const Netlist::pin_instance_name_type& pinName)
    {
        if(m_name_to_pin.find(pinName) == m_name_to_pin.end()) {
            auto pin = m_pins.add();
            m_pin_names[pin] = pinName;
            m_name_to_pin[pinName] = pin;

            return pin;
        }
        else {
            return m_name_to_pin[pinName];
        }
    }

    Netlist::net_type Netlist::add_net(const Netlist::net_name_type& netName)
    {
        if(m_name_to_net.find(netName) == m_name_to_net.end()) {
            auto net = m_nets.add();
            m_net_names[net] = netName;
            m_name_to_net[netName] = net;

            return net;
        }
        else {
            return m_name_to_net[netName];
        }
    }

    Netlist::input_pad_type Netlist::add_input_pad(const Netlist::pin_instance_type& p)
    {
        auto inp = input(p);

        if(inp != Input()) {
            return inp;
        }
        m_pin_to_input_pad.addAssociation(p, inp = m_input_pads.add());

        return inp;
    }

    Netlist::output_pad_type Netlist::add_output_pad(const Netlist::pin_instance_type& p)
    {
        auto out = output(p);

        if(out != Output()) {
            return out;
        }
        m_pin_to_output_pad.addAssociation(p, out = m_output_pads.add());

        return out;
    }

    void Netlist::erase(const Netlist::cell_instance_type& c)
    {
        m_name_to_cell.erase(m_cell_names[c]);
        m_cells.erase(c);
    }


    void Netlist::erase(const Netlist::pin_instance_type& en)
    {
        m_name_to_pin.erase(m_pin_names[en]);
        m_pins.erase(en);
    }

    void Netlist::erase(const Netlist::net_type& en)
    {
        m_name_to_net.erase(m_net_names[en]);
        m_nets.erase(en);
    }

    void Netlist::connect(const Netlist::net_type& net, const Netlist::pin_instance_type& pin)
    {
        m_net_to_pins.addAssociation(net, pin);
    }

    void Netlist::connect(const Netlist::cell_instance_type& c, const Netlist::pin_instance_type& p)
    {
        m_cell_to_pins.addAssociation(c, p);
    }

    void Netlist::connect(const Netlist::cell_instance_type& cell, const Netlist::std_cell_type& stdCell)
    {
        m_cell_instance_to_std_cell[cell] = stdCell;
    }

    void Netlist::connect(const Netlist::pin_instance_type& pin, const Netlist::std_cell_pin_type& stdCell)
    {
        m_pin_instance_to_std_cell_pin[pin] = stdCell;
    }

    void Netlist::disconnect(const Netlist::pin_instance_type& p)
    {
        m_net_to_pins.eraseAssociation(net(p), p);
    }

    entity_system::EntitySystem<PinInstance>::NotifierType * Netlist::notifier_pin_instance() const noexcept {
        return m_pins.notifier();
    }

    entity_system::EntitySystem<CellInstance>::NotifierType * Netlist::notifier_cell_instance() const noexcept {
        return m_cells.notifier();
    }

    entity_system::EntitySystem<Net>::NotifierType * Netlist::notifier_net() const noexcept {
        return m_nets.notifier();
    }

    entity_system::EntitySystem<Input>::NotifierType * Netlist::notifier_input_pad() const noexcept {
        return m_input_pads.notifier();
    }

    entity_system::EntitySystem<Output>::NotifierType * Netlist::notifier_output_pad() const noexcept {
        return m_output_pads.notifier();
    }
}
