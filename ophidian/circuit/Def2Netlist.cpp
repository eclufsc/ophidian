#include "Def2Netlist.h"

namespace ophidian
{
namespace circuit
{
    void def2LibraryMapping(
        const parser::Def & def,
        Netlist & netlist,
        StandardCells & standardCells)
    {
        for(auto & component : def.components())
        {
            auto cell = netlist.add(CellInstance(), component.name());
            auto stdCell = standardCells.add(Cell(), component.macro());
            netlist.cellStdCell(cell, stdCell);
        }
    }
}     // namespace circuit
}     // namespace ophidian
