#include <catch.hpp>

#include <ophidian/placement/Lef2Library.h>

using namespace ophidian;

class Lef2LibraryFixture
{
public:
	Lef2LibraryFixture(){
		lef = std::make_unique<ophidian::parser::Lef>();
		parser.readFile("./input_files/simple/simple.lef", lef);
		library.reset(new placement::Library(stdCells));
		placement::lef2Library(*lef, *library, stdCells);
	}
	parser::LefParser parser;
	std::unique_ptr<parser::Lef> lef;
	std::unique_ptr<placement::Library> library;
	standard_cell::StandardCells stdCells;
};

TEST_CASE_METHOD(Lef2LibraryFixture, "Lef2Library: Test library cell geometries.", "[standard_cell][library][placement][lef][dbunits]")
{
	REQUIRE(library->geometry(stdCells.find(standard_cell::Cell(), "INV_X1")) == geometry::MultiBox({geometry::Box(geometry::Point(0.0*lef->databaseUnits(), 0.0*lef->databaseUnits()),
																											   geometry::Point(0.760*lef->databaseUnits(), 1.71*lef->databaseUnits()))}));
}

TEST_CASE_METHOD(Lef2LibraryFixture, "Lef2Library: Test library pin offset.", "[standard_cell][library][placement][lef][dbunits]")
{
	REQUIRE(library->pinOffset(stdCells.find(standard_cell::Pin(), "INV_X1:a")) == util::LocationDbu(0.5*(0.210+0.340)*lef->databaseUnits(), 0.5*(0.34+0.405)*lef->databaseUnits()));
}

TEST_CASE_METHOD(Lef2LibraryFixture, "Lef2Library: Test of association between standard_cell::Pin and standard_cell::Cell.", "[circuit][standard_cell][lef][EntitySystem]")
{
	auto cellNAND2X1 = stdCells.find(standard_cell::Cell(), "NAND2_X1");
	auto pincellNAND2X1a = stdCells.find(standard_cell::Pin(), "NAND2_X1:a");
	REQUIRE(stdCells.owner(pincellNAND2X1a) == cellNAND2X1);
	REQUIRE(stdCells.pins(cellNAND2X1).size() == 3);
}
