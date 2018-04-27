#ifndef DEF2LIBRARYMAPPING_H
#define DEF2LIBRARYMAPPING_H

#include <ophidian/parser/Def.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/circuit/StandardCells.h>

namespace ophidian
{
namespace circuit
{
    void def2LibraryMapping(const parser::Def & def,
        Netlist & netlist,
        StandardCells & standardCells);
}     // namespace circuit
}     // namespace ophidian


#endif // DEF2LIBRARYMAPPING_H
