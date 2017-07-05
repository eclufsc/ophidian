#ifndef DEF2LIBRARYMAPPING_H
#define DEF2LIBRARYMAPPING_H

#include <ophidian/parser/Def.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <unordered_map>

namespace ophidian
{
namespace circuit
{

void def2LibraryMapping(const parser::Def & def, circuit::Netlist & netlist, standard_cell::StandardCells & standardCells, LibraryMapping & libraryMapping);

} // namespace circuit
} // namespace ophidian


#endif // DEF2LIBRARYMAPPING_H
