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
    Netlist::Netlist():
            mNetPins(mNets, mPins),
            mCellPins(mCells, mPins),
            mPinInput(mPins, mInputs),
            mPinOutput(mPins, mOutputs),
            mCellNames(makeProperty<std::string>(Cell())),
            mPinNames(makeProperty<std::string>(Pin())),
            mNetNames(makeProperty<std::string>(Net()))
    {
    }

    Cell Netlist::add(Cell, std::string cellName)
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

    void Netlist::erase(const Cell & c)
    {
        mName2Cell.erase(mCellNames[c]);
        mCells.erase(c);
    }

    uint32_t Netlist::size(Cell) const
    {
        return mCells.size();
    }

    Pin Netlist::add(Pin, std::string pinName)
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

    void Netlist::erase(const Pin & en)
    {
        mName2Pin.erase(mPinNames[en]);
        mPins.erase(en);
    }

    uint32_t Netlist::size(Pin) const
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

    uint32_t Netlist::size(Net) const
    {
        return mNets.size();
    }

    Net Netlist::net(const Pin & p) const
    {
        return mNetPins.whole(p);
    }

    void Netlist::disconnect(const Pin & p)
    {
        mNetPins.eraseAssociation(net(p), p);
    }

    Cell Netlist::cell(const Pin & p) const
    {
        return mCellPins.whole(p);
    }

    entity_system::EntitySystem<Pin>::NotifierType * Netlist::notifier(Pin) const {
        return mPins.notifier();
    }

    void Netlist::reserve(Pin, uint32_t size)
    {
        mPins.reserve(size);
        mName2Pin.reserve(size);
    }

    uint32_t Netlist::capacity(Pin) const
    {
        return mPins.capacity();
    }

    Pin Netlist::find(Pin, std::string pinName)
    {
        return mName2Pin[pinName];
    }

    std::string Netlist::name(const Pin & pin) const
    {
        return mPinNames[pin];
    }

    entity_system::EntitySystem<Cell>::const_iterator Netlist::begin(Cell) const
    {
        return mCells.begin();
    }

    entity_system::EntitySystem<Cell>::const_iterator Netlist::end(Cell) const
    {
        return mCells.end();
    }

    void Netlist::add(const Cell & c, const Pin & p)
    {
        mCellPins.addAssociation(c, p);
    }

    entity_system::EntitySystem<Cell>::NotifierType * Netlist::notifier(Cell) const {
        return mCells.notifier();
    }

    void Netlist::reserve(Cell, uint32_t size)
    {
        mCells.reserve(size);
        mName2Cell.reserve(size);
    }

    uint32_t Netlist::capacity(Cell) const
    {
        return mCells.capacity();
    }

    Cell Netlist::find(Cell, std::string cellName)
    {
        return mName2Cell[cellName];
    }

    std::string Netlist::name(const Cell & cell) const
    {
        return mCellNames[cell];
    }

    entity_system::Association<Cell, Pin>::Parts Netlist::pins(const Cell & cell) const
    {
        return mCellPins.parts(cell);
    }

    entity_system::EntitySystem<Pin>::const_iterator Netlist::begin(Pin) const
    {
        return mPins.begin();
    }

    entity_system::EntitySystem<Pin>::const_iterator Netlist::end(Pin) const
    {
        return mPins.end();
    }

    entity_system::EntitySystem<Net>::const_iterator Netlist::begin(Net) const
    {
        return mNets.begin();
    }

    entity_system::EntitySystem<Net>::const_iterator Netlist::end(Net) const
    {
        return mNets.end();
    }

    void Netlist::connect(const Net & net, const Pin & pin)
    {
        mNetPins.addAssociation(net, pin);
    }

    entity_system::EntitySystem<Net>::NotifierType * Netlist::notifier(Net) const {
        return mNets.notifier();
    }

    void Netlist::reserve(Net, uint32_t size)
    {
        mNets.reserve(size);
        mName2Net.reserve(size);
    }

    uint32_t Netlist::capacity(Net) const
    {
        return mNets.capacity();
    }

    Net Netlist::find(Net, std::string netName)
    {
        return mName2Net[netName];
    }

    std::string Netlist::name(const Net & net) const
    {
        return mNetNames[net];
    }

    entity_system::Association<Net, Pin>::Parts Netlist::pins(const Net & net) const
    {
        return mNetPins.parts(net);
    }

    uint32_t Netlist::size(Input) const
    {
        return mInputs.size();
    }

    Input Netlist::add(Input, const Pin & p)
    {
        Input inp = input(p);

        if(inp != Input()) {
            return inp;
        }
        mPinInput.addAssociation(p, inp = mInputs.add());

        return inp;
    }

    Pin Netlist::pin(const Input & input) const
    {
        return mPinInput.whole(input);
    }

    Input Netlist::input(const Pin & pin) const
    {
        return mPinInput.firstPart(pin);
    }

    entity_system::EntitySystem<Input>::const_iterator Netlist::begin(Input) const
    {
        return mInputs.begin();
    }

    entity_system::EntitySystem<Input>::const_iterator Netlist::end(Input) const
    {
        return mInputs.end();
    }

    entity_system::EntitySystem<Input>::NotifierType * Netlist::notifier(Input) const {
        return mInputs.notifier();
    }

    uint32_t Netlist::size(Output) const
    {
        return mOutputs.size();
    }

    Output Netlist::add(Output, const Pin & p)
    {
        Output out = output(p);

        if(out != Output()) {
            return out;
        }
        mPinOutput.addAssociation(p, out = mOutputs.add());

        return out;
    }

    Pin Netlist::pin(const Output & output) const
    {
        return mPinOutput.whole(output);
    }

    Output Netlist::output(const Pin & pin) const
    {
        return mPinOutput.firstPart(pin);
    }

    entity_system::EntitySystem<Output>::const_iterator Netlist::begin(Output) const
    {
        return mOutputs.begin();
    }

    entity_system::EntitySystem<Output>::const_iterator Netlist::end(Output) const
    {
        return mOutputs.end();
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
}     // namespace circuit
}     // namespace ophidian
