#include <catch.hpp>

#include <ophidian/circuit/Netlist.h>

using namespace ophidian::circuit;

TEST_CASE("Netlist: Brand-new Netlist must be empty.", "[circuit][Netlist]")
{
	Netlist nl;
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
	REQUIRE(nl.size(Input()) == 0 );
	REQUIRE(nl.size(Output()) == 0 );

}

TEST_CASE("Netlist: Add Cell.", "[circuit][Netlist]")
{
	Netlist nl;
    auto cell = nl.add(CellInstance(), "cell");
    REQUIRE(nl.size(CellInstance()) == 1);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Erase Cell.", "[circuit][Netlist]")
{

	Netlist nl;
    auto cell = nl.add(CellInstance(), "cell");
	nl.erase(cell);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Cell mapping.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add(CellInstance(), "cell");
    auto cell = nl.find_cell_instance("cell");
	nl.erase(cell);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Try add the same cell twice.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add(CellInstance(), "cell");
    nl.add(CellInstance(), "cell");
    REQUIRE(nl.size(CellInstance()) == 1);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Add Pin.", "[circuit][Netlist]")
{
	Netlist nl;
    auto pin = nl.add(PinInstance(), "pin");
	REQUIRE(nl.net(pin) == Net());
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 1);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Erase Pin.", "[circuit][Netlist]")
{
	Netlist nl;
    auto pin = nl.add(PinInstance(), "pin");
	nl.erase(pin);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Pin mapping.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add(PinInstance(), "pin");
    auto pin = nl.find_pin_instance("pin");
	nl.erase(pin);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Try add the same pin twice.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add(PinInstance(), "pin");
    nl.add(PinInstance(), "pin");
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 1);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Add Net.", "[circuit][Netlist]")
{
	Netlist nl;
	auto net = nl.add(Net(), "net");
	REQUIRE(nl.pins(net).empty());
	REQUIRE(nl.pins(net).size() == 0);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 1);
}

TEST_CASE("Netlist: Erase Net.", "[circuit][Netlist]")
{
	Netlist nl;
	auto net = nl.add(Net(), "net");
	nl.erase(net);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Net mapping.", "[circuit][Netlist]")
{
	Netlist nl;
	nl.add(Net(), "net");
	auto net = nl.find_net("net");
	nl.erase(net);
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 0);
}

TEST_CASE("Netlist: Try add the same net twice.", "[circuit][Netlist]")
{
	Netlist nl;
	nl.add(Net(), "net");
	nl.add(Net(), "net");
    REQUIRE(nl.size(CellInstance()) == 0);
    REQUIRE(nl.size(PinInstance()) == 0);
	REQUIRE(nl.size(Net()) == 1);
}

TEST_CASE("Netlist: Connect/Disconnect Net and Pin.", "[circuit][Netlist]")
{

	Netlist nl;
	auto net = nl.add(Net(), "net");
    auto pin = nl.add(PinInstance(), "pin");
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
    auto cell = nl.add(CellInstance(), "cell");
    auto pin = nl.add(PinInstance(), "cell:pin");
	nl.add(cell, pin);
	REQUIRE(nl.pins(cell).size());
	REQUIRE(std::count(nl.pins(cell).begin(), nl.pins(cell).end(), pin) == 1);
	REQUIRE(nl.cell(pin) == cell);
}

TEST_CASE("Netlist: Make Cell Property (names)", "[circuit][Netlist]")
{
	Netlist nl;
    auto u1 = nl.add(CellInstance(), "u1");
    auto u2  = nl.add(CellInstance(), "u2");
    auto names = nl.makeProperty<std::string>(CellInstance());
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
    auto u1_a = nl.add(PinInstance(), "u1_a");
    auto u1_b  = nl.add(PinInstance(), "u1_b");
    auto positions = nl.makeProperty<Point2D>(PinInstance());
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
	auto n1 = nl.add(Net(), "n1");
	auto n2  = nl.add(Net(), "n2");
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
		Netlist::CellNotifier::ObserverBase(*nl.notifier(CellInstance())),
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
	void add(const CellInstance&) override
	{
		++added;
	}
	void add(const std::vector<CellInstance>& cells) override
	{
		added += cells.size();
	}
	void erase(const CellInstance&) override
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
    auto n1 = nl.add(CellInstance(), "n1");
    auto n2  = nl.add(CellInstance(), "n2");
    auto n3 = nl.add(CellInstance(), "n3");
	nl.erase(n2);
	REQUIRE( prop.added == 3 );
	REQUIRE( prop.erased == 1 );
    nl.reserve(CellInstance(), 3214);
	REQUIRE( prop.capacity == 3214 );
}


TEST_CASE("Netlist: Add Input.", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance p1 = nl.add(PinInstance(), "p1");
    PinInstance p2 = nl.add(PinInstance(), "p2");
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
    PinInstance p1 = nl.add(PinInstance(), "p1");
	REQUIRE(nl.output(p1) == Output());
	auto out = nl.add(Output(), p1 );
	REQUIRE(nl.size(Output()) == 1);
	REQUIRE(nl.pin(out) == p1);
	REQUIRE(nl.output(p1) == out);
}

TEST_CASE("Netlist: Input & Output ranges.", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance p1, p2;
    auto inp = nl.add(Input(), p1 = nl.add(PinInstance(), "p1"));
    auto out = nl.add(Output(), p2 = nl.add(PinInstance(), "p2"));
	REQUIRE(std::count(nl.begin_input_pad(), nl.end_input_pad(), inp) == 1);
	REQUIRE(std::count(nl.begin_output_pad(), nl.end_output_pad(), out) == 1);
}

TEST_CASE("Netlist: Input Slews & Output Loads", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance inp1, inp2;
    PinInstance out;

    inp1 = nl.add(PinInstance(), "inp1");
    inp2 = nl.add(PinInstance(), "inp2");
    out = nl.add(PinInstance(), "out");

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
