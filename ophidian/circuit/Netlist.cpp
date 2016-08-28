#include "Netlist.h"

namespace ophidian {
namespace circuit {

Netlist::Netlist() :
    netPins_(nets_, pins_),
    cellPins_(cells_, pins_),
    pinInput_(pins_, inputs_),
    pinOutput_(pins_, outputs_)
{
}

Netlist::Netlist(Netlist &&nl) :
    cells_(std::move(nl.cells_)),
    pins_(std::move(nl.pins_)),
    nets_(std::move(nl.nets_)),
    inputs_(std::move(nl.inputs_)),
    netPins_(std::move(nl.netPins_)),
    cellPins_(std::move(nl.cellPins_)),
    pinInput_(std::move(nl.pinInput_)),
    pinOutput_(std::move(nl.pinOutput_))

{

}

Netlist::~Netlist()
{

}

Cell Netlist::add(Cell)
{
    return cells_.add();
}

void Netlist::erase(const Cell &c)
{
    cells_.erase(c);
}

uint32_t Netlist::size(Cell) const
{
    return cells_.size();
}

Pin Netlist::add(Pin)
{
    return pins_.add();
}

void Netlist::erase(const Pin &en)
{
    pins_.erase(en);
}

uint32_t Netlist::size(Pin) const
{
    return pins_.size();
}

Net Netlist::add(Net)
{
    return nets_.add();
}

void Netlist::erase(const Net &en)
{
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
    netPins_.erasePart(net(p), p);
}

Cell Netlist::cell(const Pin &p) const
{
    return cellPins_.whole(p);
}

entity_system::EntitySystem<Pin>::NotifierType *Netlist::notifier(Pin) const
{
    return pins_.notifier();
}

const entity_system::Aggregation<Net, Pin>::PartContainer &Netlist::pins(const Net &n) const
{
    return netPins_.parts(n);
}

entity_system::EntitySystem<Cell>::const_iterator Netlist::begin(Cell) const
{
    return cells_.begin();
}

entity_system::EntitySystem<Cell>::const_iterator Netlist::end(Cell) const
{
    return cells_.end();
}

const entity_system::Composition<Cell, Pin>::PartContainer &Netlist::pins(const Cell &c) const
{
    return cellPins_.parts(c);
}

void Netlist::add(const Cell &c, const Pin &p)
{
    cellPins_.addPart(c, p);
}

entity_system::EntitySystem<Cell>::NotifierType *Netlist::notifier(Cell) const
{
    return cells_.notifier();
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
    netPins_.addPart(net, pin);
}

entity_system::EntitySystem<Net>::NotifierType *Netlist::notifier(Net) const
{
    return nets_.notifier();
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
    pinInput_.addPart(p, inp = inputs_.add());
    return inp;
}

Pin Netlist::pin(const Input &input) const
{
    return pinInput_.whole(input);
}

Input Netlist::input(const Pin &pin) const
{
    if(pinInput_.parts(pin).empty())
        return Input();
    return pinInput_.parts(pin).front();
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
    pinOutput_.addPart(p, out = outputs_.add());
    return out;
}

Pin Netlist::pin(const Output &output) const
{
    return pinOutput_.whole(output);
}

Output Netlist::output(const Pin &pin) const
{
    if(pinOutput_.parts(pin).empty())
        return Output();
    return pinOutput_.parts(pin).front();
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

}
}

