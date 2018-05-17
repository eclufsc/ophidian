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

namespace ophidian
{
namespace circuit
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

        template <class T> using entity_container_type = entity_system::EntitySystem<T>;

        using size_type = std::size_t;

        using cell_entity_type = CellInstance;
        using cell_entity_container_type = entity_container_type<cell_entity_type>;

        using cell_name_type   = std::string;

        using net_entity_type = Net;
        using net_name_type   = std::string;

        using pin_entity_type = PinInstance;
        using pin_name_type   = std::string;

        using input_pad_entity_type = Input;
        using input_pad_name_type   = std::string;

        using output_pad_entity_type = Output;
        using output_pad_name_type   = std::string;

        //! Construct Netlist
        Netlist() = default;

        //! Delete coppy constructor
        Netlist(const Netlist & nl) = delete;
        Netlist & operator =(const Netlist & nl) = delete;

        //! Move Constructor
        Netlist(Netlist &&) = default;
        Netlist& operator=(Netlist &&) = default;

        // Element access
        CellInstance find(CellInstance, std::string cellName);
        PinInstance find(PinInstance, std::string pinName);
        Net find(Net, std::string netName);

        std::string name(const CellInstance & cell) const;
        std::string name(const PinInstance & pin) const;
        std::string name(const Net & net) const;

        entity_system::Association<CellInstance, PinInstance>::Parts pins(const CellInstance & cell) const;
        entity_system::Association<Net, PinInstance>::Parts pins(const Net & net) const;
        Net net(const PinInstance & pin) const;

        CellInstance cell(const PinInstance & pin) const;

        PinInstance pin(const Input & input) const;

        Input input(const PinInstance & pin) const;

        PinInstance pin(const Output & output) const;

        Output output(const PinInstance & pin) const;

        Cell cellStdCell(const CellInstance & cell) const;
        Pin pinStdCell(const PinInstance & pin) const;

        // Iterators
        cell_entity_container_type::const_iterator begin(cell_entity_type) const;
        cell_entity_container_type::const_iterator end(cell_entity_type) const;

        entity_system::EntitySystem<PinInstance>::const_iterator begin(PinInstance) const;
        entity_system::EntitySystem<PinInstance>::const_iterator end(PinInstance) const;

        entity_system::EntitySystem<Net>::const_iterator begin(Net) const;
        entity_system::EntitySystem<Net>::const_iterator end(Net) const;

        entity_system::EntitySystem<Input>::const_iterator begin(Input) const;
        entity_system::EntitySystem<Input>::const_iterator end(Input) const;
        
        entity_system::EntitySystem<Output>::const_iterator begin(Output) const;
        entity_system::EntitySystem<Output>::const_iterator end(Output) const;

        // Capacity
        size_type size(cell_entity_type) const;
        size_type size(PinInstance) const;
        size_type size(Net) const;
        size_type size(Input) const;
        size_type size(Output) const;

        void reserve(CellInstance, size_type size);
        void reserve(PinInstance, size_type size);
        void reserve(Net, size_type size);

        void shrinkToFit();

        size_type capacity(CellInstance) const;
        size_type capacity(PinInstance) const;
        size_type capacity(Net) const;

        // Modifiers
        void erase(const cell_entity_type& cell);
        void erase(const PinInstance & pin);
        void erase(const Net & net);

        void disconnect(const PinInstance & pin);
        void connect(const Net & net, const PinInstance & pin);

        void add(const CellInstance & cell, const PinInstance & pin);
        void cellStdCell(const CellInstance & cell, const Cell & stdCell);
        void pinStdCell(const PinInstance & pin, const Pin & stdCell);

        cell_entity_type add(cell_entity_type, cell_name_type cellName);
        Net add(Net, std::string netName);
        PinInstance add(PinInstance, std::string pinName);
        Input add(Input, const PinInstance &pin);
        Output add(Output, const PinInstance &pin);
        

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
        entity_system::Property<CellInstance, std::string>    mCellNames{makeProperty<std::string>(CellInstance())};
        entity_system::Property<PinInstance, std::string>     mPinNames{makeProperty<std::string>(PinInstance())};
        entity_system::Property<Net, std::string>             mNetNames{makeProperty<std::string>(Net())};
        std::unordered_map<std::string, CellInstance>         mName2Cell{};
        std::unordered_map<std::string, PinInstance>          mName2Pin{};
        std::unordered_map<std::string, Net>                  mName2Net{};
        entity_system::Aggregation<Net, PinInstance>          mNetPins{mNets, mPins};
        entity_system::Composition<CellInstance, PinInstance> mCellPins{mCells, mPins};
        entity_system::Composition<PinInstance, Input>        mPinInput{mPins, mInputs};
        entity_system::Composition<PinInstance, Output>       mPinOutput{mPins, mOutputs};

        entity_system::Property<CellInstance, Cell> cells2StdCells_{};
        entity_system::Property<PinInstance, Pin>   pins2StdCells_{};
    };
}     // namespace circuit
}     // namespace ophidian

#endif // OPHIDIAN_CIRCUIT_NETLIST_H
