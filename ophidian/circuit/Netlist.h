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
        entity_system::Property<CellInstance, Value> makeProperty(CellInstance) const
        {
            return entity_system::Property<CellInstance, Value>(mCells);
        }

        template <typename Value>
        entity_system::Property<PinInstance, Value> makeProperty(PinInstance) const
        {
            return entity_system::Property<PinInstance, Value>(mPins);
        }

        template <typename Value>
        entity_system::Property<Net, Value> makeProperty(Net) const
        {
            return entity_system::Property<Net, Value>(mNets);
        }

        template <typename Value>
        entity_system::Property<Input, Value> makeProperty(Input) const
        {
            return entity_system::Property<Input, Value>(mInputs);
        }

        template <typename Value>
        entity_system::Property<Output, Value> makeProperty(Output) const
        {
            return entity_system::Property<Output, Value>(mOutputs);
        }

        entity_system::EntitySystem<CellInstance>::NotifierType * notifier(CellInstance) const;
        entity_system::EntitySystem<PinInstance>::NotifierType * notifier(PinInstance) const;
        entity_system::EntitySystem<Net>::NotifierType * notifier(Net) const;
        entity_system::EntitySystem<Input>::NotifierType * notifier(Input) const;
        entity_system::EntitySystem<Output>::NotifierType * notifier(Output) const;

    private:
        entity_system::EntitySystem<CellInstance>             mCells{};
        entity_system::EntitySystem<PinInstance>              mPins{};
        entity_system::EntitySystem<Net>                      mNets{};
        entity_system::EntitySystem<Input>                    mInputs{};
        entity_system::EntitySystem<Output>                   mOutputs{};
        entity_system::Property<CellInstance, std::string>    mCellNames{mCells};
        entity_system::Property<PinInstance, std::string>     mPinNames{mPins};
        entity_system::Property<Net, std::string>             mNetNames{mNets};
        std::unordered_map<std::string, CellInstance>         mName2Cell{};
        std::unordered_map<std::string, PinInstance>          mName2Pin{};
        std::unordered_map<std::string, Net>                  mName2Net{};
        entity_system::Aggregation<Net, PinInstance>          mNetPins{mNets, mPins};

        entity_system::Composition<CellInstance, PinInstance> mCellPins{mCells, mPins};
        entity_system::Composition<PinInstance, Input>        mPinInput{mPins, mInputs};
        entity_system::Composition<PinInstance, Output>       mPinOutput{mPins, mOutputs};

        entity_system::Property<CellInstance, Cell> cells2StdCells_{mCells};
        entity_system::Property<PinInstance, Pin>   pins2StdCells_{mPins};
    };
}

#endif // OPHIDIAN_CIRCUIT_NETLIST_H
