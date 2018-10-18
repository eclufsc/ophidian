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
        return mName2Cell.at(cellName);
    }

    Netlist::pin_instance_type Netlist::find_pin_instance(const Netlist::pin_instance_name_type& pinName) const
    {
        return mName2Pin.at(pinName);
    }

    Netlist::net_type Netlist::find_net(const Netlist::net_name_type& netName) const
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

    ophidian::util::Range<Netlist::cell_instance_container_type::const_iterator> Netlist::range_cell() const noexcept
    {
        return ophidian::util::Range<Netlist::cell_instance_container_type::const_iterator>(mCells.begin(), mCells.end());
    }

    ophidian::util::Range<Netlist::net_container_type::const_iterator> Netlist::range_net() const noexcept
    {
        return ophidian::util::Range<Netlist::net_container_type::const_iterator>(mNets.begin(), mNets.end());
    }

    Netlist::cell_instance_container_type::size_type Netlist::size_cell_instance() const noexcept
    {
        return mCells.size();
    }

    Netlist::pin_instance_container_type::size_type Netlist::size_pin_instance() const noexcept
    {
        return mPins.size();
    }

    Netlist::net_container_type::size_type Netlist::size_net() const noexcept
    {
        return mNets.size();
    }

    Netlist::input_pad_container_type::size_type Netlist::size_input_pad() const noexcept
    {
        return mInputs.size();
    }

    Netlist::output_pad_container_type::size_type Netlist::size_output_pad() const noexcept
    {
        return mOutputs.size();
    }

    Netlist::cell_instance_container_type::size_type Netlist::capacity_cell_instance() const noexcept
    {
        return mCells.capacity();
    }

    Netlist::pin_instance_container_type::size_type Netlist::capacity_pin_instance() const noexcept
    {
        return mPins.capacity();
    }

    Netlist::net_container_type::size_type Netlist::capacity_net() const noexcept
    {
        return mNets.capacity();
    }

    void Netlist::reserve_cell_instance(Netlist::cell_instance_container_type::size_type size)
    {
        mCells.reserve(size);
        mName2Cell.reserve(size);
    }

    void Netlist::reserve_pin_instance(Netlist::pin_instance_container_type::size_type size)
    {
        mPins.reserve(size);
        mName2Pin.reserve(size);
    }

    void Netlist::reserve_net(Netlist::net_container_type::size_type size)
    {
        mNets.reserve(size);
        mName2Net.reserve(size);
    }

    void Netlist::shrink_to_fit()
    {
        mCells.shrinkToFit();
        mPins.shrinkToFit();
        mNets.shrinkToFit();
        mInputs.shrinkToFit();
        mOutputs.shrinkToFit();
    }

    Netlist::cell_instance_type Netlist::add_cell_instance(const Netlist::cell_instance_name_type& cellName)
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

    Netlist::pin_instance_type Netlist::add_pin_instance(const Netlist::pin_instance_name_type& pinName)
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

    Netlist::net_type Netlist::add_net(const Netlist::net_name_type& netName)
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

    Netlist::input_pad_type Netlist::add_input_pad(const Netlist::pin_instance_type& p)
    {
        auto inp = input(p);

        if(inp != Input()) {
            return inp;
        }
        mPinInput.addAssociation(p, inp = mInputs.add());

        return inp;
    }

    Netlist::output_pad_type Netlist::add_output_pad(const Netlist::pin_instance_type& p)
    {
        auto out = output(p);

        if(out != Output()) {
            return out;
        }
        mPinOutput.addAssociation(p, out = mOutputs.add());

        return out;
    }

    void Netlist::erase(const Netlist::cell_instance_type& c)
    {
        mName2Cell.erase(mCellNames[c]);
        mCells.erase(c);
    }


    void Netlist::erase(const Netlist::pin_instance_type& en)
    {
        mName2Pin.erase(mPinNames[en]);
        mPins.erase(en);
    }

    void Netlist::erase(const Netlist::net_type& en)
    {
        mName2Net.erase(mNetNames[en]);
        mNets.erase(en);
    }

    void Netlist::connect(const Netlist::net_type& net, const Netlist::pin_instance_type& pin)
    {
        mNetPins.addAssociation(net, pin);
    }

    void Netlist::connect(const Netlist::cell_instance_type& c, const Netlist::pin_instance_type& p)
    {
        mCellPins.addAssociation(c, p);
    }

    void Netlist::connect(const Netlist::cell_instance_type& cell, const Netlist::std_cell_type& stdCell)
    {
        cells2StdCells_[cell] = stdCell;
    }

    void Netlist::connect(const Netlist::pin_instance_type& pin, const Netlist::std_cell_pin_type& stdCell)
    {
        pins2StdCells_[pin] = stdCell;
    }

    void Netlist::disconnect(const Netlist::pin_instance_type& p)
    {
        mNetPins.eraseAssociation(net(p), p);
    }

    entity_system::EntitySystem<PinInstance>::NotifierType * Netlist::notifier(PinInstance) const {
        return mPins.notifier();
    }

    entity_system::EntitySystem<CellInstance>::NotifierType * Netlist::notifier(CellInstance) const {
        return mCells.notifier();
    }

    entity_system::EntitySystem<Net>::NotifierType * Netlist::notifier(Net) const {
        return mNets.notifier();
    }

    entity_system::EntitySystem<Input>::NotifierType * Netlist::notifier(Input) const {
        return mInputs.notifier();
    }

    entity_system::EntitySystem<Output>::NotifierType * Netlist::notifier(Output) const {
        return mOutputs.notifier();
    }
}
