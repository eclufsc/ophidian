#include "Def2LibraryMapping.h"

namespace ophidian
{
    namespace circuit
    {
        void def2LibraryMapping(
            const parser::Def & def,
            circuit::Netlist & netlist,
            standard_cell::StandardCells & standardCells,
            LibraryMapping & libraryMapping)
        {
            for(auto & component : def.components())
            {
                auto cell = netlist.add(Cell(), component.name);
                auto stdCell = standardCells.add(standard_cell::Cell(), component.macro);
                libraryMapping.cellStdCell(cell, stdCell);
            }
        }
    }     // namespace circuit
}     // namespace ophidian