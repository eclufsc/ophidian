#include "Verilog2LibraryMapping.h"

namespace ophidian
{
namespace circuit
{

void verilog2LibraryMapping(const parser::Verilog & verilog, circuit::Netlist & netlist, standard_cell::StandardCells & standardCells, LibraryMapping & libraryMapping)
{
    const parser::Verilog::Module & module = verilog.modules().front();
    for(auto & instance : module.instances())
    {
        auto cell = netlist.add(ophidian::circuit::Cell(), instance.name());
        auto stdCell = standardCells.add(standard_cell::Cell(), instance.module()->name());
        libraryMapping.cellStdCell(cell, stdCell);
    }
}

} // namespace circuit
} // namespace ophidian
