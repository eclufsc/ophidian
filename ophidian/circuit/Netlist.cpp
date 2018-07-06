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

namespace ophidian
{
namespace circuit
{
    const Netlist::cell_instance_type& Netlist::find_cell_instance(const Netlist::cell_instance_name_type& cellName) const
    {
        return mName2Cell.at(cellName);
    }

    const Netlist::pin_instance_type& Netlist::find_pin_instance(const Netlist::pin_instance_name_type& pinName) const
    {
        return mName2Pin.at(pinName);
    }

    const Netlist::net_type& Netlist::find_net(const Netlist::net_name_type& netName) const
    {
        return mName2Net.at(netName);
    }

    Netlist::cell_instance_name_type& Netlist::name(const Netlist::cell_instance_type& cell)
    {
        return mCellNames[cell];
    }

    const Netlist::cell_instance_name_type& Netlist::name(const Netlist::cell_instance_type& cell) const
    {
        return mCellNames[cell];
    }

    Netlist::pin_instance_name_type& Netlist::name(const Netlist::pin_instance_type& pin)
    {
        return mPinNames[pin];
    }

    const Netlist::pin_instance_name_type& Netlist::name(const Netlist::pin_instance_type& pin) const
    {
        return mPinNames[pin];
    }

    Netlist::net_name_type& Netlist::name(const Netlist::net_type& net)
    {
        return mNetNames[net];
    }

    const Netlist::net_name_type& Netlist::name(const Netlist::net_type& net) const
    {
        return mNetNames[net];
    }

    Netlist::cell_instance_type Netlist::cell(const Netlist::pin_instance_type& pin) const
    {
        return mCellPins.whole(pin);
    }

    Netlist::cell_instance_pins_view_type Netlist::pins(const Netlist::cell_instance_type& cell) const
    {
        return mCellPins.parts(cell);
    }

    Netlist::net_pins_view_type Netlist::pins(const Netlist::net_type& net) const
    {
        return mNetPins.parts(net);
    }

    Netlist::pin_instance_type Netlist::pin(const Netlist::input_pad_type& input) const
    {
        return mPinInput.whole(input);
    }

    Netlist::pin_instance_type Netlist::pin(const Netlist::output_pad_type& output) const
    {
        return mPinOutput.whole(output);
    }

    Netlist::net_type Netlist::net(const Netlist::pin_instance_type& pin) const
    {
        return mNetPins.whole(pin);
    }

    Netlist::input_pad_type Netlist::input(const Netlist::pin_instance_type& pin) const
    {
        return mPinInput.firstPart(pin);
    }

    Netlist::output_pad_type Netlist::output(const Netlist::pin_instance_type& pin) const
    {
        return mPinOutput.firstPart(pin);
    }

    Netlist::std_cell_type Netlist::std_cell(const Netlist::cell_instance_type& cell) const
    {
        return cells2StdCells_[cell];
    }

    Netlist::std_cell_pin_type Netlist::std_cell_pin(const Netlist::pin_instance_type& pin) const
    {
        return pins2StdCells_[pin];
    }

    Netlist::cell_instance_container_type::const_iterator Netlist::begin_cell_instance() const noexcept
    {
        return mCells.begin();
    }

    Netlist::cell_instance_container_type::const_iterator Netlist::end_cell_instance() const noexcept
    {
        return mCells.end();
    }

    Netlist::pin_instance_container_type::const_iterator Netlist::begin_pin_instance() const noexcept
    {
        return mPins.begin();
    }

    Netlist::pin_instance_container_type::const_iterator Netlist::end_pin_instance() const noexcept
    {
        return mPins.end();
    }

    Netlist::net_container_type::const_iterator Netlist::begin_net() const noexcept
    {
        return mNets.begin();
    }

    Netlist::net_container_type::const_iterator Netlist::end_net() const noexcept
    {
        return mNets.end();
    }

    Netlist::input_pad_container_type::const_iterator Netlist::begin_input_pad() const noexcept
    {
        return mInputs.begin();
    }

    Netlist::input_pad_container_type::const_iterator Netlist::end_input_pad() const noexcept
    {
        return mInputs.end();
    }

    Netlist::output_pad_container_type::const_iterator Netlist::begin_output_pad() const noexcept
    {
        return mOutputs.begin();
    }

    Netlist::output_pad_container_type::const_iterator Netlist::end_output_pad() const noexcept
    {
        return mOutputs.end();
    }

    CellInstance Netlist::add(CellInstance, std::string cellName)
    {
        if(mName2Cell.find(cellName) == mName2Cell.end()) {
            auto cell = mCells.add();
            mCellNames[cell] = cellName;
            mName2Cell[cellName] = cell;

            return cell;
        }
        else {
            return mName2Cell[cellName];
        }
    }

    void Netlist::erase(const CellInstance & c)
    {
        mName2Cell.erase(mCellNames[c]);
        mCells.erase(c);
    }

    Netlist::size_type Netlist::size(CellInstance) const
    {
        return mCells.size();
    }

    PinInstance Netlist::add(PinInstance, std::string pinName)
    {
        if(mName2Pin.find(pinName) == mName2Pin.end()) {
            auto pin = mPins.add();
            mPinNames[pin] = pinName;
            mName2Pin[pinName] = pin;

            return pin;
        }
        else {
            return mName2Pin[pinName];
        }
    }

    void Netlist::erase(const PinInstance & en)
    {
        mName2Pin.erase(mPinNames[en]);
        mPins.erase(en);
    }

    Netlist::size_type Netlist::size(PinInstance) const
    {
        return mPins.size();
    }

    Net Netlist::add(Net, std::string netName)
    {
        if(mName2Net.find(netName) == mName2Net.end()) {
            auto net = mNets.add();
            mNetNames[net] = netName;
            mName2Net[netName] = net;

            return net;
        }
        else {
            return mName2Net[netName];
        }
    }

    void Netlist::erase(const Net & en)
    {
        mName2Net.erase(mNetNames[en]);
        mNets.erase(en);
    }

    Netlist::size_type Netlist::size(Net) const
    {
        return mNets.size();
    }

    void Netlist::disconnect(const PinInstance & p)
    {
        mNetPins.eraseAssociation(net(p), p);
    }

    entity_system::EntitySystem<PinInstance>::NotifierType * Netlist::notifier(PinInstance) const {
        return mPins.notifier();
    }

    void Netlist::reserve(PinInstance, Netlist::size_type size)
    {
        mPins.reserve(size);
        mName2Pin.reserve(size);
    }

    Netlist::size_type Netlist::capacity(PinInstance) const
    {
        return mPins.capacity();
    }

    void Netlist::add(const CellInstance & c, const PinInstance & p)
    {
        mCellPins.addAssociation(c, p);
    }

    entity_system::EntitySystem<CellInstance>::NotifierType * Netlist::notifier(CellInstance) const {
        return mCells.notifier();
    }

    void Netlist::reserve(CellInstance, Netlist::size_type size)
    {
        mCells.reserve(size);
        mName2Cell.reserve(size);
    }

    Netlist::size_type Netlist::capacity(CellInstance) const
    {
        return mCells.capacity();
    }

    void Netlist::connect(const Net & net, const PinInstance & pin)
    {
        mNetPins.addAssociation(net, pin);
    }

    entity_system::EntitySystem<Net>::NotifierType * Netlist::notifier(Net) const {
        return mNets.notifier();
    }

    void Netlist::reserve(Net, Netlist::size_type size)
    {
        mNets.reserve(size);
        mName2Net.reserve(size);
    }

    Netlist::size_type Netlist::capacity(Net) const
    {
        return mNets.capacity();
    }

    Netlist::size_type Netlist::size(Input) const
    {
        return mInputs.size();
    }

    Input Netlist::add(Input, const PinInstance & p)
    {
        Input inp = input(p);

        if(inp != Input()) {
            return inp;
        }
        mPinInput.addAssociation(p, inp = mInputs.add());

        return inp;
    }

    entity_system::EntitySystem<Input>::NotifierType * Netlist::notifier(Input) const {
        return mInputs.notifier();
    }

    Netlist::size_type Netlist::size(Output) const
    {
        return mOutputs.size();
    }

    Output Netlist::add(Output, const PinInstance & p)
    {
        Output out = output(p);

        if(out != Output()) {
            return out;
        }
        mPinOutput.addAssociation(p, out = mOutputs.add());

        return out;
    }

    entity_system::EntitySystem<Output>::NotifierType * Netlist::notifier(Output) const {
        return mOutputs.notifier();
    }

    void Netlist::shrinkToFit()
    {
        mCells.shrinkToFit();
        mPins.shrinkToFit();
        mNets.shrinkToFit();
        mInputs.shrinkToFit();
        mOutputs.shrinkToFit();
    }

    void Netlist::cellStdCell(const CellInstance & cell, const Cell & stdCell)
    {
        cells2StdCells_[cell] = stdCell;
    }

    void Netlist::pinStdCell(const PinInstance & pin, const Pin & stdCell)
    {
        pins2StdCells_[pin] = stdCell;
    }
}     // namespace circuit
}     // namespace ophidian
