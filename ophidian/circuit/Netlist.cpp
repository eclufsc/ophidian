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

namespace ophidian {
namespace circuit {

Netlist::Netlist() :
    netPins_(nets_, pins_),
    cellPins_(cells_, pins_),
    pinInput_(pins_, inputs_),
    pinOutput_(pins_, outputs_),
    cellNames_(makeProperty<std::string>(Cell())),
    pinNames_(makeProperty<std::string>(Pin())),
    netNames_(makeProperty<std::string>(Net()))
{
}

Netlist::~Netlist()
{

}

Cell Netlist::add(Cell, std::string cellName)
{
    auto cell = cells_.add();
    cellNames_[cell] = cellName;
    name2Cell_[cellName] = cell;
    return cell;
}

void Netlist::erase(const Cell &c)
{
    name2Cell_.erase(cellNames_[c]);
    cells_.erase(c);
}

uint32_t Netlist::size(Cell) const
{
    return cells_.size();
}

Pin Netlist::add(Pin, std::string pinName)
{
    auto pin = pins_.add();
    pinNames_[pin] = pinName;
    name2Pin_[pinName] = pin;
    return pin;
}

void Netlist::erase(const Pin &en)
{
    name2Pin_.erase(pinNames_[en]);
    pins_.erase(en);
}

uint32_t Netlist::size(Pin) const
{
    return pins_.size();
}

Net Netlist::add(Net, std::string netName)
{
    auto net = nets_.add();
    netNames_[net] = netName;
    name2Net_[netName] = net;
    return net;
}

void Netlist::erase(const Net &en)
{
    name2Net_.erase(netNames_[en]);
    nets_.erase(en);
}

uint32_t Netlist::size(Net) const
{
    return nets_.size();
}

Net Netlist::net(const Pin &p) const
{
    return netPins_.whole(p);
}

void Netlist::disconnect(const Pin &p)
{
    netPins_.eraseAssociation(net(p), p);
}

Cell Netlist::cell(const Pin &p) const
{
    return cellPins_.whole(p);
}

entity_system::EntitySystem<Pin>::NotifierType *Netlist::notifier(Pin) const
{
    return pins_.notifier();
}

void Netlist::reserve(Pin, uint32_t size)
{
    pins_.reserve(size);
    name2Pin_.reserve(size);
}

uint32_t Netlist::capacity(Pin) const
{
    return pins_.capacity();
}

Pin Netlist::find(Pin, std::string pinName)
{
    return name2Pin_[pinName];
}

std::string Netlist::name(const Pin& pin) const
{
    return pinNames_[pin];
}

entity_system::EntitySystem<Cell>::const_iterator Netlist::begin(Cell) const
{
    return cells_.begin();
}

entity_system::EntitySystem<Cell>::const_iterator Netlist::end(Cell) const
{
    return cells_.end();
}

void Netlist::add(const Cell &c, const Pin &p)
{
    cellPins_.addAssociation(c, p);
}

entity_system::EntitySystem<Cell>::NotifierType *Netlist::notifier(Cell) const
{
    return cells_.notifier();
}

void Netlist::reserve(Cell, uint32_t size)
{
    cells_.reserve(size);
    name2Cell_.reserve(size);
}

uint32_t Netlist::capacity(Cell) const
{
    return cells_.capacity();
}

Cell Netlist::find(Cell, std::string cellName)
{
    return name2Cell_[cellName];
}

std::string Netlist::name(const Cell& cell) const
{
    return cellNames_[cell];
}

entity_system::Association<Cell, Pin>::Parts Netlist::pins(const Cell &cell) const
{
    return cellPins_.parts(cell);
}

entity_system::EntitySystem<Pin>::const_iterator Netlist::begin(Pin) const
{
    return pins_.begin();
}

entity_system::EntitySystem<Pin>::const_iterator Netlist::end(Pin) const
{
    return pins_.end();
}

entity_system::EntitySystem<Net>::const_iterator Netlist::begin(Net) const
{
    return nets_.begin();
}

entity_system::EntitySystem<Net>::const_iterator Netlist::end(Net) const
{
    return nets_.end();
}

void Netlist::connect(const Net &net, const Pin &pin)
{
    netPins_.addAssociation(net, pin);
}

entity_system::EntitySystem<Net>::NotifierType *Netlist::notifier(Net) const
{
    return nets_.notifier();
}

void Netlist::reserve(Net, uint32_t size)
{
    nets_.reserve(size);
    name2Net_.reserve(size);
}

uint32_t Netlist::capacity(Net) const
{
    return nets_.capacity();
}

Net Netlist::find(Net, std::string netName)
{
    return name2Net_[netName];
}

std::string Netlist::name(const Net& net) const
{
    return netNames_[net];
}

entity_system::Association<Net, Pin>::Parts Netlist::pins(const Net &net) const
{
    return netPins_.parts(net);
}

uint32_t Netlist::size(Input) const
{
    return inputs_.size();
}

Input Netlist::add(Input, const Pin &p)
{
    Input inp = input(p);
    if(inp != Input())
        return inp;
    pinInput_.addAssociation(p, inp = inputs_.add());
    return inp;
}

Pin Netlist::pin(const Input &input) const
{
    return pinInput_.whole(input);
}

Input Netlist::input(const Pin &pin) const
{
    return pinInput_.firstPart(pin);
}

entity_system::EntitySystem<Input>::const_iterator Netlist::begin(Input) const
{
    return inputs_.begin();
}

entity_system::EntitySystem<Input>::const_iterator Netlist::end(Input) const
{
    return inputs_.end();
}

entity_system::EntitySystem<Input>::NotifierType *Netlist::notifier(Input) const
{
    return inputs_.notifier();
}

uint32_t Netlist::size(Output) const
{
    return outputs_.size();
}

Output Netlist::add(Output, const Pin &p)
{
    Output out = output(p);
    if(out != Output())
        return out;
    pinOutput_.addAssociation(p, out = outputs_.add());
    return out;
}

Pin Netlist::pin(const Output &output) const
{
    return pinOutput_.whole(output);
}

Output Netlist::output(const Pin &pin) const
{
    return pinOutput_.firstPart(pin);
}

entity_system::EntitySystem<Output>::const_iterator Netlist::begin(Output) const
{
    return outputs_.begin();
}

entity_system::EntitySystem<Output>::const_iterator Netlist::end(Output) const
{
    return outputs_.end();
}

entity_system::EntitySystem<Output>::NotifierType *Netlist::notifier(Output) const
{
    return outputs_.notifier();
}

void Netlist::shrinkToFit()
{
    cells_.shrinkToFit();
    pins_.shrinkToFit();
    nets_.shrinkToFit();
    inputs_.shrinkToFit();
    outputs_.shrinkToFit();
}

}
}

