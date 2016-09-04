#include "netlist_profiling.h"
#include <catch.hpp>

#include <ophidian/circuit/Netlist.h>
#include <array>
#include <random>

using namespace ophidian::circuit;

TEST_CASE("Netlist: Profiling", "[Netlist][Profiling]")
{
    Netlist nl;
    const std::uint32_t kNumCells = 1000000;
    nl.reserve(Cell(), kNumCells);
    nl.reserve(Pin(), 4*kNumCells);
    std::vector<Cell> cells(kNumCells);
    auto cellNames = nl.makeProperty<std::string>(Cell());
    for(uint32_t i = 0; i < cells.size(); ++i)
    {
        cells[i] = nl.add(Cell());
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
            nl.add(cell, nl.add(Pin()));
        }
    }
    nl.shrink();
}
