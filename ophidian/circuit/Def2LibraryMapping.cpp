#include "Def2LibraryMapping.h"

namespace ophidian
{
namespace circuit
{

void def2LibraryMapping(const parser::Def & def, circuit::Netlist & netlist, standard_cell::StandardCells & standardCells, LibraryMapping & libraryMapping)
{
    for (auto & component : def.components())
    {
        auto cell = netlist.add(Cell(), component.name);
        auto stdCell = standardCells.add(standard_cell::Cell(), component.macro);
        libraryMapping.cellStdCell(cell, stdCell);

        for(auto & pin : netlist.pins(cell))
        {
            std::string pinName = netlist.name(pin);
            std::string stdName = component.macro + pinName.substr(pinName.find(":"));
            auto stdPin = standardCells.find(standard_cell::Pin(), stdName);
            libraryMapping.pinStdCell(pin, stdPin);
        }
    }

    for (auto inputIt = netlist.begin(Input()); inputIt != netlist.end(Input()); ++inputIt)
    {
        auto pinInput = netlist.pin(*inputIt);
        std::string inputName = netlist.name(pinInput);
        auto stdInput = standardCells.add(standard_cell::Pin(), inputName, standard_cell::PinDirection::OUTPUT);
        libraryMapping.pinStdCell(pinInput, stdInput);
    }

    for (auto outputIt = netlist.begin(Output()); outputIt != netlist.end(Output()); ++outputIt)
    {
        auto pinOutput = netlist.pin(*outputIt);
        std::string outputName = netlist.name(pinOutput);
        auto stdOutput = standardCells.add(standard_cell::Pin(), outputName, standard_cell::PinDirection::INPUT);
        libraryMapping.pinStdCell(pinOutput, stdOutput);
    }
}

} // namespace circuit
} // namespace ophidian
