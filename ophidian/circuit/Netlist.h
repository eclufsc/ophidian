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

#ifndef OPHIDIAN_CIRCUIT_NETLIST_H
#define OPHIDIAN_CIRCUIT_NETLIST_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/entity_system/Composition.h>
#include <ophidian/circuit/StandardCells.h>
#include <unordered_map>

namespace ophidian::circuit
{
    class CellInstance :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class PinInstance :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Net :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Input :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Output :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    /*! A flatten Netlist */
    class Netlist final
    {
    public:
        using CellNotifier = entity_system::EntitySystem<CellInstance>::NotifierType;
        using PinNotifier = entity_system::EntitySystem<PinInstance>::NotifierType;
        using NetNotifier = entity_system::EntitySystem<Net>::NotifierType;

        using cell_instance_type = CellInstance;
        using cell_instance_name_type = std::string;
        using cell_instance_container_type = entity_system::EntitySystem<cell_instance_type>;

        using pin_instance_type = PinInstance;
        using pin_instance_name_type = std::string;
        using pin_instance_container_type = entity_system::EntitySystem<pin_instance_type>;

        using net_type = Net;
        using net_name_type = std::string;
        using net_container_type = entity_system::EntitySystem<net_type>;

        using input_pad_type = Input;
        using input_pad_name_type   = std::string;
        using input_pad_container_type = entity_system::EntitySystem<Input>;

        using output_pad_type = Output;
        using output_pad_name_type   = std::string;
        using output_pad_container_type = entity_system::EntitySystem<Output>;

        using cell_instance_pins_view_type = entity_system::Association<CellInstance, PinInstance>::Parts;

        using net_pins_view_type = entity_system::Association<Net, PinInstance>::Parts;

        using std_cell_type = StandardCells::cell_type;
        using std_cell_pin_type = StandardCells::pin_type;

        //! Construct Netlist
        Netlist() = default;

        //! Delete coppy constructor
        Netlist(const Netlist &) = delete;
        Netlist & operator =(const Netlist &) = delete;

        //! Move Constructor
        Netlist(Netlist &&) = delete;
        Netlist& operator=(Netlist &&) = delete;

        // Element access
        cell_instance_type find_cell_instance(const cell_instance_name_type& cellName) const;

        pin_instance_type find_pin_instance(const pin_instance_name_type& pinName) const;

        net_type find_net(const net_name_type& netName) const;

        cell_instance_name_type& name(const cell_instance_type& cell);
        const cell_instance_name_type& name(const cell_instance_type& cell) const;

        pin_instance_name_type& name(const pin_instance_type& pin);
        const pin_instance_name_type& name(const pin_instance_type& pin) const;

        net_name_type& name(const net_type& net);
        const net_name_type& name(const net_type& net) const;

        cell_instance_type cell(const pin_instance_type& pin) const;

        cell_instance_pins_view_type pins(const cell_instance_type& cell) const;

        net_pins_view_type pins(const net_type& net) const;

        pin_instance_type pin(const input_pad_type& input) const;

        pin_instance_type pin(const output_pad_type& output) const;

        net_type net(const pin_instance_type& pin) const;

        input_pad_type input(const pin_instance_type& pin) const;

        output_pad_type output(const pin_instance_type& pin) const;

        std_cell_type std_cell(const cell_instance_type& cell) const;

        std_cell_pin_type std_cell_pin(const pin_instance_type& pin) const;

        // Iterators
        cell_instance_container_type::const_iterator begin_cell_instance() const noexcept;
        cell_instance_container_type::const_iterator end_cell_instance() const noexcept;

        pin_instance_container_type::const_iterator begin_pin_instance() const noexcept;
        pin_instance_container_type::const_iterator end_pin_instance() const noexcept;

        net_container_type::const_iterator begin_net() const noexcept;
        net_container_type::const_iterator end_net() const noexcept;

        input_pad_container_type::const_iterator begin_input_pad() const noexcept;
        input_pad_container_type::const_iterator end_input_pad() const noexcept;

        output_pad_container_type::const_iterator begin_output_pad() const noexcept;
        output_pad_container_type::const_iterator end_output_pad() const noexcept;

        // Capacity
        cell_instance_container_type::size_type size_cell_instance() const noexcept;
        pin_instance_container_type::size_type size_pin_instance() const noexcept;
        net_container_type::size_type size_net() const noexcept;
        input_pad_container_type::size_type size_input_pad() const noexcept;
        output_pad_container_type::size_type size_output_pad() const noexcept;

        cell_instance_container_type::size_type capacity_cell_instance() const noexcept;
        pin_instance_container_type::size_type capacity_pin_instance() const noexcept;
        net_container_type::size_type capacity_net() const noexcept;

        void reserve_cell_instance(cell_instance_container_type::size_type size);
        void reserve_pin_instance(pin_instance_container_type::size_type size);
        void reserve_net(net_container_type::size_type size);

        void shrink_to_fit();

        // Modifiers
        cell_instance_type add_cell_instance(const cell_instance_name_type& cellName);

        pin_instance_type add_pin_instance(const pin_instance_name_type& pinName);

        net_type add_net(const net_name_type& netName);

        input_pad_type add_input_pad(const pin_instance_type& pin);

        output_pad_type add_output_pad(const pin_instance_type& pin);

        void erase(const cell_instance_type& cell);
        void erase(const pin_instance_type& pin);
        void erase(const net_type& net);

        void connect(const net_type& net, const pin_instance_type& pin);
        void connect(const cell_instance_type& cell, const pin_instance_type& pin);
        void connect(const cell_instance_type& cell, const std_cell_type& std_cell);
        void connect(const pin_instance_type& pin, const std_cell_pin_type& std_cell);

        void disconnect(const pin_instance_type& pin);

        template <typename Value>
        entity_system::Property<CellInstance, Value> make_property_cell_instance() const noexcept
        {
            return entity_system::Property<CellInstance, Value>(m_cells);
        }

        template <typename Value>
        entity_system::Aggregation<CellInstance, Value> make_aggregation_cell(entity_system::EntitySystem<Value> & parts) const noexcept
        {
            return entity_system::Aggregation<CellInstance, Value>(m_cells, parts);
        }

        template <typename Value>
        entity_system::Property<PinInstance, Value> make_property_pin_instance() const noexcept
        {
            return entity_system::Property<PinInstance, Value>(m_pins);
        }

        template <typename Value>
        entity_system::Property<Net, Value> make_property_net() const noexcept
        {
            return entity_system::Property<Net, Value>(m_nets);
        }

        template <typename Value>
        entity_system::Property<Net, Value> make_property_net(Value default_value) const noexcept
        {
            return entity_system::Property<Net, Value>(m_nets, default_value);
        }

        template <typename Value>
        entity_system::Aggregation<Net, Value> make_aggregation_net(entity_system::EntitySystem<Value> & parts) const noexcept
        {
            return entity_system::Aggregation<Net, Value>(m_nets, parts);
        }

        template <typename Value>
        entity_system::Property<Input, Value> make_property_input_pad() const noexcept
        {
            return entity_system::Property<Input, Value>(m_input_pads);
        }

        template <typename Value>
        entity_system::Property<Output, Value> make_property_output_pad() const noexcept
        {
            return entity_system::Property<Output, Value>(m_output_pads);
        }

        entity_system::EntitySystem<CellInstance>::NotifierType * notifier_cell_instance() const noexcept;
        entity_system::EntitySystem<PinInstance>::NotifierType * notifier_pin_instance() const noexcept;
        entity_system::EntitySystem<Net>::NotifierType * notifier_net() const noexcept;
        entity_system::EntitySystem<Input>::NotifierType * notifier_input_pad() const noexcept;
        entity_system::EntitySystem<Output>::NotifierType * notifier_output_pad() const noexcept;

    private:
        entity_system::EntitySystem<CellInstance>             m_cells{};
        entity_system::EntitySystem<PinInstance>              m_pins{};
        entity_system::EntitySystem<Net>                      m_nets{};
        entity_system::EntitySystem<Input>                    m_input_pads{};
        entity_system::EntitySystem<Output>                   m_output_pads{};
        entity_system::Property<CellInstance, std::string>    m_cell_names{m_cells};
        entity_system::Property<PinInstance, std::string>     m_pin_names{m_pins};
        entity_system::Property<Net, std::string>             m_net_names{m_nets};
        std::unordered_map<std::string, CellInstance>         m_name_to_cell{};
        std::unordered_map<std::string, PinInstance>          m_name_to_pin{};
        std::unordered_map<std::string, Net>                  m_name_to_net{};
        entity_system::Aggregation<Net, PinInstance>          m_net_to_pins{m_nets, m_pins};

        entity_system::Composition<CellInstance, PinInstance> m_cell_to_pins{m_cells, m_pins};
        entity_system::Composition<PinInstance, Input>        m_pin_to_input_pad{m_pins, m_input_pads};
        entity_system::Composition<PinInstance, Output>       m_pin_to_output_pad{m_pins, m_output_pads};

        entity_system::Property<CellInstance, Cell>           m_cell_instance_to_std_cell{m_cells};
        entity_system::Property<PinInstance, Pin>             m_pin_instance_to_std_cell_pin{m_pins};
    };
}

#endif // OPHIDIAN_CIRCUIT_NETLIST_H
