#ifndef NETLIST_H
#define NETLIST_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/entity_system/Composition.h>

namespace ophidian {
namespace circuit {

class Cell : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

class Pin : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

class Net : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

class Input : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

class Output : public entity_system::EntityBase {
public:
    using entity_system::EntityBase::EntityBase;
};

template <class Entity_>
class Manager {
public:
    virtual ~Manager() { }
    virtual Entity_ add(Entity_) = 0;
    virtual void erase(const Entity_ & en) = 0;
    virtual uint32_t size(Entity_) const = 0;
};

class Netlist final : public Manager<Cell>,
                      public Manager<Pin>,
                      public Manager<Net>
{
public:
    using CellNotifier = entity_system::EntitySystem<Cell>::NotifierType;
    using PinNotifier = entity_system::EntitySystem<Pin>::NotifierType;
    using NetNotifier = entity_system::EntitySystem<Net>::NotifierType;

    Netlist();
    Netlist(Netlist && nl);
    ~Netlist() override;

    Cell add(Cell) override;
    void erase(const Cell& en) override;
    uint32_t size(Cell) const override;
    entity_system::EntitySystem<Cell>::const_iterator begin(Cell) const;
    entity_system::EntitySystem<Cell>::const_iterator end(Cell) const;
    const entity_system::Composition<Cell, Pin>::PartContainer & pins(const Cell& c) const;
    void add(const Cell& c, const Pin &p);
    template <typename Value>
    entity_system::Property<Cell, Value> makeProperty(Cell) {
        return entity_system::Property<Cell, Value>(cells_);
    }
    entity_system::EntitySystem<Cell>::NotifierType* notifier(Cell) const;

    Pin add(Pin) override;
    void erase(const Pin& en) override;
    uint32_t size(Pin) const override;
    entity_system::EntitySystem<Pin>::const_iterator begin(Pin) const;
    entity_system::EntitySystem<Pin>::const_iterator end(Pin) const;
    Net net(const Pin & p) const;
    void disconnect(const Pin & p);
    Cell cell(const Pin & p) const;
    template <typename Value>
    entity_system::Property<Pin, Value> makeProperty(Pin) {
        return entity_system::Property<Pin, Value>(pins_);
    }
    entity_system::EntitySystem<Pin>::NotifierType* notifier(Pin) const;

    Net add(Net) override;
    void erase(const Net& en) override;
    uint32_t size(Net) const override;
    entity_system::EntitySystem<Net>::const_iterator begin(Net) const;
    entity_system::EntitySystem<Net>::const_iterator end(Net) const;
    const entity_system::Aggregation<Net, Pin>::PartContainer & pins(const Net & n) const;
    void connect(const Net & net, const Pin & pin);
    template <typename Value>
    entity_system::Property<Net, Value> makeProperty(Net) {
        return entity_system::Property<Net, Value>(nets_);
    }
    entity_system::EntitySystem<Net>::NotifierType* notifier(Net) const;

    uint32_t size(Input) const;
    Input add(Input, const Pin & p);
    Pin pin(const Input & input) const;
    Input input(const Pin & pin) const;
    entity_system::EntitySystem<Input>::const_iterator begin(Input) const;
    entity_system::EntitySystem<Input>::const_iterator end(Input) const;

    uint32_t size(Output) const;
    Output add(Output, const Pin & p);
    Pin pin(const Output & output) const;
    Output output(const Pin & pin) const;
    entity_system::EntitySystem<Output>::const_iterator begin(Output) const;
    entity_system::EntitySystem<Output>::const_iterator end(Output) const;

private:
    Netlist(const Netlist& nl) = delete;
    Netlist& operator =(const Netlist& nl) = delete;
    entity_system::EntitySystem<Cell> cells_;
    entity_system::EntitySystem<Pin> pins_;
    entity_system::EntitySystem<Net> nets_;
    entity_system::EntitySystem<Input> inputs_;
    entity_system::EntitySystem<Output> outputs_;
    entity_system::Aggregation<Net, Pin> netPins_;
    entity_system::Composition<Cell, Pin> cellPins_;
    entity_system::Composition<Pin, Input> pinInput_;
    entity_system::Composition<Pin, Output> pinOutput_;
};

}
}

#endif // NETLIST_H
