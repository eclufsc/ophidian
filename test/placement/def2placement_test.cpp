#include <catch.hpp>

#include <ophidian/placement/Def2Placement.h>

using namespace ophidian;
using dbu_t = ophidian::util::database_unit_t;

namespace {
bool operator==(const ophidian::util::LocationDbu& a, const ophidian::util::LocationDbu& b) noexcept
{
    return a.x() == b.x() && a.y() == b.y();
}

bool operator!=(const ophidian::util::LocationDbu& a, const ophidian::util::LocationDbu& b) noexcept
{
    return !(a == b);
}
}

class Def2PlacementFixture
{
public:
	Def2PlacementFixture() : placement(netlist){
		def = std::make_unique<parser::Def>("./input_files/simple/simple.def");
	}

	std::unique_ptr<parser::Def> def;
	circuit::Netlist netlist;
	placement::Placement placement;
};

TEST_CASE_METHOD(Def2PlacementFixture, "Def2Placement: Test if def2placement mapped cells's position correctly according .def file.", "[placement][Def]")
{
	ophidian::placement::def2placement(*def, placement, netlist);

    REQUIRE(placement.cellLocation(netlist.find(circuit::Cell(), "u1")) == util::LocationDbu{dbu_t{3420}, dbu_t{6840}});
    REQUIRE(placement.cellLocation(netlist.find(circuit::Cell(), "u2")) == util::LocationDbu{dbu_t{3420}, dbu_t{3420}});
    REQUIRE(placement.cellLocation(netlist.find(circuit::Cell(), "u3")) == util::LocationDbu{dbu_t{6840}, dbu_t{3420}});
    REQUIRE(placement.cellLocation(netlist.find(circuit::Cell(), "u4")) == util::LocationDbu{dbu_t{12160}, dbu_t{6840}});
    REQUIRE(placement.cellLocation(netlist.find(circuit::Cell(), "lcb1")) == util::LocationDbu{dbu_t{0}, dbu_t{10260}});
}
