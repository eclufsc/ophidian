#include <catch.hpp>

#include <ophidian/circuit/Netlist.h>
#include <array>
#include <random>
#include <string>

using namespace ophidian::circuit;

TEST_CASE("Netlist: Profiling", "[Netlist][Profiling]")
{
	Netlist nl;
	const std::uint32_t kNumCells = 1000000;
    nl.reserve(CellInstance(), kNumCells);
    nl.reserve(PinInstance(), 4*kNumCells);
    std::vector<CellInstance> cells(kNumCells);
    auto cellsPins = nl.makeProperty<std::vector<PinInstance> >(CellInstance());
    auto cellNames = nl.makeProperty<std::string>(CellInstance());
	for(uint32_t i = 0; i < cells.size(); ++i)
	{
        cells[i] = nl.add(CellInstance(), "cell_" + std::to_string(i));
		cellNames[cells[i]] = "cell_" + std::to_string(i);
	}
	std::default_random_engine engine;
	std::random_shuffle(cells.begin(), cells.end());
	for(auto const & cell : cells)
	{
		std::normal_distribution<double> dist(3.0, .8);
		uint32_t cellPins = static_cast<uint32_t>(std::min(std::max(1.0, dist(engine)), 4.0));
//        std::cout << "cell " << cellNames[cell] << " will have " << cellPins << " pins" << std::endl;
		for(uint32_t i = 0; i < cellPins; ++i)
		{
            auto pin = nl.add(PinInstance(), cellNames[cell]+":"+std::to_string(i));
			nl.add(cell, pin);
			cellsPins[cell].push_back(pin);
		}
	}

    std::list<CellInstance> cellsWithoutTheExpectedPins;
    std::for_each(nl.begin(CellInstance()), nl.end(CellInstance()), [&cellsPins, &nl, &cellsWithoutTheExpectedPins](const CellInstance & cell){
		auto const & pins = cellsPins[cell];
		auto nlCellPins = nl.pins(cell);
		bool insert = false;
        std::for_each(nlCellPins.begin(), nlCellPins.end(), [&insert, &pins](const PinInstance & pin){
			insert = insert || (std::count(pins.begin(), pins.end(), pin) != 1);
		});
		if(insert)
		{
		    cellsWithoutTheExpectedPins.push_back(cell);
		}
	});

	REQUIRE( cellsWithoutTheExpectedPins.empty() );
	nl.shrinkToFit();
    REQUIRE( nl.capacity(PinInstance()) == nl.size(PinInstance()) );
	REQUIRE( nl.capacity(Net()) == nl.size(Net()) );
    REQUIRE( nl.capacity(CellInstance()) == nl.size(CellInstance()) );
}
