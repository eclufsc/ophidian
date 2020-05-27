#include <catch.hpp>

#include <ophidian/circuit/Netlist.h>

using namespace ophidian::circuit;

TEST_CASE("Netlist: Brand-new Netlist must be empty.", "[circuit][Netlist]")
{
	Netlist nl;
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
	REQUIRE(nl.size_pad() == 0 );
}

TEST_CASE("Netlist: Add Cell.", "[circuit][Netlist]")
{
	Netlist nl;
    auto cell = nl.add_cell_instance("cell");
    REQUIRE(nl.size_cell_instance() == 1);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Erase Cell.", "[circuit][Netlist]")
{

	Netlist nl;
    auto cell = nl.add_cell_instance("cell");
	nl.erase(cell);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Cell mapping.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add_cell_instance("cell");
    auto cell = nl.find_cell_instance("cell");
	nl.erase(cell);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Try add the same cell twice.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add_cell_instance("cell");
    nl.add_cell_instance("cell");
    REQUIRE(nl.size_cell_instance() == 1);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Add Pin.", "[circuit][Netlist]")
{
	Netlist nl;
    auto pin = nl.add_pin_instance("pin");
	REQUIRE(nl.net(pin) == Net());
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 1);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Erase Pin.", "[circuit][Netlist]")
{
	Netlist nl;
    auto pin = nl.add_pin_instance("pin");
	nl.erase(pin);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Pin mapping.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add_pin_instance("pin");
    auto pin = nl.find_pin_instance("pin");
	nl.erase(pin);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Try add the same pin twice.", "[circuit][Netlist]")
{
	Netlist nl;
    nl.add_pin_instance("pin");
    nl.add_pin_instance("pin");
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 1);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Add Net.", "[circuit][Netlist]")
{
	Netlist nl;
	auto net = nl.add_net("net");
	REQUIRE(nl.pins(net).empty());
	REQUIRE(nl.pins(net).size() == 0);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 1);
}

TEST_CASE("Netlist: Erase Net.", "[circuit][Netlist]")
{
	Netlist nl;
	auto net = nl.add_net("net");
	nl.erase(net);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Net mapping.", "[circuit][Netlist]")
{
	Netlist nl;
	nl.add_net("net");
	auto net = nl.find_net("net");
	nl.erase(net);
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 0);
}

TEST_CASE("Netlist: Try add the same net twice.", "[circuit][Netlist]")
{
	Netlist nl;
	nl.add_net("net");
	nl.add_net("net");
    REQUIRE(nl.size_cell_instance() == 0);
    REQUIRE(nl.size_pin_instance() == 0);
	REQUIRE(nl.size_net() == 1);
}

TEST_CASE("Netlist: Connect/Disconnect Net and Pin.", "[circuit][Netlist]")
{

	Netlist nl;
	auto net = nl.add_net("net");
    auto pin = nl.add_pin_instance("pin");
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
    auto cell = nl.add_cell_instance("cell");
    auto pin = nl.add_pin_instance("cell:pin");
	nl.connect(cell, pin);
	REQUIRE(nl.pins(cell).size());
	REQUIRE(std::count(nl.pins(cell).begin(), nl.pins(cell).end(), pin) == 1);
	REQUIRE(nl.cell(pin) == cell);
}

TEST_CASE("Netlist: Make Cell Property (names)", "[circuit][Netlist]")
{
	Netlist nl;
    auto u1 = nl.add_cell_instance("u1");
    auto u2  = nl.add_cell_instance("u2");
    auto names = nl.make_property_cell_instance<std::string>();
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
    auto u1_a = nl.add_pin_instance("u1_a");
    auto u1_b  = nl.add_pin_instance("u1_b");
    auto positions = nl.make_property_pin_instance<Point2D>();
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
	auto n1 = nl.add_net("n1");
	auto n2  = nl.add_net("n2");
	auto CLoad = nl.make_property_net<Capacitance>();
	CLoad[n1] = 1e-15;
	CLoad[n2] = 2e-15;
	REQUIRE(Approx(CLoad[n1]) == 1e-15);
	REQUIRE(Approx(CLoad[n2]) == 2e-15);
}

class DummyCellObserver : public Netlist::CellNotifier::ObserverBase
{
public:
	DummyCellObserver(const Netlist & nl) :
		Netlist::CellNotifier::ObserverBase(*nl.notifier_cell_instance()),
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
    auto n1 = nl.add_cell_instance("n1");
    auto n2  = nl.add_cell_instance("n2");
    auto n3 = nl.add_cell_instance("n3");
	nl.erase(n2);
	REQUIRE( prop.added == 3 );
	REQUIRE( prop.erased == 1 );
    nl.reserve_cell_instance(3214);
	REQUIRE( prop.capacity == 3214 );
}


TEST_CASE("Netlist: Add Input. and Output", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance p1 = nl.add_pin_instance("p1");
    PinInstance p2 = nl.add_pin_instance("p2");
	auto inp1 = nl.add_pad(p1);
	nl.set_direction(inp1, Netlist::pad_direction_type::INPUT);
	REQUIRE(nl.size_pad() == 1);
	REQUIRE(nl.direction(inp1) == Netlist::pad_direction_type::INPUT);
	auto inp2 = nl.add_pad(p2 );
	REQUIRE(nl.size_pad() == 2);
	nl.set_direction(inp2, Netlist::pad_direction_type::OUTPUT);
	REQUIRE(nl.direction(inp2) == Netlist::pad_direction_type::OUTPUT);
	REQUIRE( nl.pin(inp1) == p1 );
	REQUIRE( nl.pin(inp2) == p2 );
}

TEST_CASE("Netlist: Input & Output ranges.", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance p1, p2;
    auto inp = nl.add_pad(p1 = nl.add_pin_instance("p1"));
    auto out = nl.add_pad(p2 = nl.add_pin_instance("p2"));
	REQUIRE(std::count(nl.begin_pad(), nl.end_pad(), inp) == 2);
}

TEST_CASE("Netlist: Input Slews & Output Loads", "[circuit][Netlist]")
{
	Netlist nl;
    PinInstance inp1, inp2;
    PinInstance out;

    inp1 = nl.add_pin_instance("inp1");
    inp2 = nl.add_pin_instance("inp2");
    out = nl.add_pin_instance("out");

	auto pad_inp1 = nl.add_pad(inp1);
	auto pad_inp2 = nl.add_pad(inp2);
	auto pad_out = nl.add_pad(out);
	nl.set_direction(pad_inp1, Netlist::pad_direction_type::INPUT);
	nl.set_direction(pad_inp2, Netlist::pad_direction_type::INPUT);
	nl.set_direction(pad_out, Netlist::pad_direction_type::OUTPUT);

	auto inputSlews = nl.make_property_pad<double>();
	auto outputLoads = nl.make_property_pad<double>();

	inputSlews[nl.pad(inp1)] = 1.1;
	inputSlews[nl.pad(inp2)] = 2.2;

	nl.erase(inp2);

	REQUIRE( inputSlews.size() == 1 );
	REQUIRE( Approx(inputSlews[nl.pad(inp1)]) == 1.1 );

}
