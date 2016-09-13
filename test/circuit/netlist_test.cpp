#include "netlist_test.h"
#include <catch.hpp>

#include <ophidian/circuit/Netlist.h>

using namespace ophidian::circuit;

TEST_CASE("Netlist: Brand-new Netlist must be empty.", "[circuit][Netlist]")
{
    Netlist nl;
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 0);
    REQUIRE(nl.size(Input()) == 0 );
    REQUIRE(nl.size(Output()) == 0 );

}

TEST_CASE("Netlist: Add Cell.", "[circuit][Netlist]")
{
    Netlist nl;
    auto cell = nl.add(Cell());
    REQUIRE(nl.size(Cell()) == 1);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Erase Cell.", "[circuit][Netlist]")
{

    Netlist nl;
    auto cell = nl.add(Cell());
    nl.erase(cell);
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Add Pin.", "[circuit][Netlist]")
{
    Netlist nl;
    auto pin = nl.add(Pin());
    REQUIRE(nl.net(pin) == Net());
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 1);
    REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Erase Pin.", "[circuit][Netlist]")
{
    Netlist nl;
    auto pin = nl.add(Pin());
    nl.erase(pin);
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Add Net.", "[circuit][Netlist]")
{
    Netlist nl;
    auto net = nl.add(Net());
    REQUIRE(nl.pins(net).empty());
    REQUIRE(nl.pins(net).size() == 0);
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 1);
}

TEST_CASE("Netlist: Erase Net.", "[circuit][Netlist]")
{
    Netlist nl;
    auto net = nl.add(Net());
    nl.erase(net);
    REQUIRE(nl.size(Cell()) == 0);
    REQUIRE(nl.size(Pin()) == 0);
    REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Connect/Disconnect Net and Pin.", "[circuit][Netlist]")
{

    Netlist nl;
    auto net = nl.add(Net());
    auto pin = nl.add(Pin());
    nl.connect(net, pin);
    REQUIRE(nl.pins(net).size() == 1);
    REQUIRE(std::count(nl.pins(net).begin(), nl.pins(net).end(), pin) == 1);
    REQUIRE(nl.net(pin) == net);
    nl.disconnect(pin);
    REQUIRE(std::count(nl.pins(net).begin(), nl.pins(net).end(), pin) == 0);
    REQUIRE(nl.net(pin) == Net());
}

TEST_CASE("Netlist: Add Pin Into Cell.", "[circuit][Netlist]")
{
    Netlist nl;
    auto cell = nl.add(Cell());
    auto pin = nl.add(Pin());
    nl.add(cell, pin);
    REQUIRE(nl.pins(cell).size());
    REQUIRE(std::count(nl.pins(cell).begin(), nl.pins(cell).end(), pin) == 1);
    REQUIRE(nl.cell(pin) == cell);
}

TEST_CASE("Netlist: Make Cell Property (names)", "[circuit][Netlist]")
{
    Netlist nl;
    auto u1 = nl.add(Cell());
    auto u2  = nl.add(Cell());
    auto names = nl.makeProperty<std::string>(Cell());
    names[u1] = "u1";
    names[u2] = "u2";
    REQUIRE(names[u1] == "u1");
    REQUIRE(names[u2] == "u2");
}

struct Point2D
{
    int x;
    int y;
};

TEST_CASE("Netlist: Make Pin Property (position).", "[circuit][Netlist]")
{
    Netlist nl;
    auto u1_a = nl.add(Pin());
    auto u1_b  = nl.add(Pin());
    auto positions = nl.makeProperty<Point2D>(Pin());
    positions[u1_a] = {0, 1};
    positions[u1_b] = {2, 3};
    REQUIRE(positions[u1_a].x == 0);
    REQUIRE(positions[u1_a].y == 1);
    REQUIRE(positions[u1_b].x == 2);
    REQUIRE(positions[u1_b].y == 3);
}

using Capacitance = double;

TEST_CASE("Netlist: Make Net Property (Lumped Capacitance).", "[circuit][Netlist]")
{
    Netlist nl;
    auto n1 = nl.add(Net());
    auto n2  = nl.add(Net());
    auto CLoad = nl.makeProperty<Capacitance>(Net());
    CLoad[n1] = 1e-15;
    CLoad[n2] = 2e-15;
    REQUIRE(Approx(CLoad[n1]) == 1e-15);
    REQUIRE(Approx(CLoad[n2]) == 2e-15);
}

class DummyCellObserver : public Netlist::CellNotifier::ObserverBase
{
    public:
        DummyCellObserver(const Netlist & nl) :
            Netlist::CellNotifier::ObserverBase(*nl.notifier(Cell())),
            added(0),
            erased(0),
            capacity(0)
        {

        }
        ~DummyCellObserver() {

        }
        void shrinkToFit() override
        {
            capacity = added - erased;
        }
        void reserve(uint32_t size) override
        {
            capacity = std::max(capacity, size);
        }
        void add(const Cell&) override
        {
            ++added;
        }
        void add(const std::vector<Cell>& cells) override
        {
            added += cells.size();
        }
        void erase(const Cell&) override
        {
            ++erased;
        }
        void clear() override
        {
            erased += added;
        }
        uint32_t added;
        uint32_t erased;
        uint32_t capacity;
};

TEST_CASE("Netlist: Make Custom Cell Observer.", "[circuit][Netlist]")
{
    Netlist nl;
    auto prop = DummyCellObserver(nl);
    auto n1 = nl.add(Cell());
    auto n2  = nl.add(Cell());
    auto n3 = nl.add(Cell());
    nl.erase(n2);
    REQUIRE( prop.added == 3 );
    REQUIRE( prop.erased == 1 );
    nl.reserve(Cell(), 3214);
    REQUIRE( prop.capacity == 3214 );
}


TEST_CASE("Netlist: Add Input.", "[circuit][Netlist]")
{
    Netlist nl;
    Pin p1 = nl.add(Pin());
    Pin p2 = nl.add(Pin());
    auto inp1 = nl.add(Input(), p1 );
    REQUIRE(nl.size(Input()) == 1);
    auto inp2 = nl.add(Input(), p2 );
    REQUIRE(nl.size(Input()) == 2);
    REQUIRE( nl.pin(inp1) == p1 );
    REQUIRE( nl.pin(inp2) == p2 );
}

TEST_CASE("Netlist: Add Output.", "[circuit][Netlist]")
{
    Netlist nl;
    Pin p1 = nl.add(Pin());
    REQUIRE(nl.output(p1) == Output());
    auto out = nl.add(Output(), p1 );
    REQUIRE(nl.size(Output()) == 1);
    REQUIRE(nl.pin(out) == p1);
    REQUIRE(nl.output(p1) == out);
}

TEST_CASE("Netlist: Input & Output ranges.", "[circuit][Netlist]")
{
    Netlist nl;
    Pin p1, p2;
    auto inp = nl.add(Input(), p1 = nl.add(Pin()));
    auto out = nl.add(Output(), p2 = nl.add(Pin()));
    REQUIRE(std::count(nl.begin(Input()), nl.end(Input()), inp) == 1);
    REQUIRE(std::count(nl.begin(Output()), nl.end(Output()), out) == 1);
}

TEST_CASE("Netlist: Input Slews & Output Loads", "[circuit][Netlist]")
{
    Netlist nl;
    Pin inp1, inp2;
    Pin out;

    inp1 = nl.add(Pin());
    inp2 = nl.add(Pin());
    out = nl.add(Pin());

    nl.add(Input(), inp1);
    nl.add(Input(), inp2);
    nl.add(Output(), out);

    auto inputSlews = nl.makeProperty<double>(Input());
    auto outputLoads = nl.makeProperty<double>(Output());

    inputSlews[nl.input(inp1)] = 1.1;
    inputSlews[nl.input(inp2)] = 2.2;

    nl.erase(inp2);

    REQUIRE( inputSlews.size() == 1 );
    REQUIRE( Approx(inputSlews[nl.input(inp1)]) == 1.1 );

}
