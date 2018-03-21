#ifndef DEF2LIBRARYMAPPING_H
#define DEF2LIBRARYMAPPING_H

#include <ophidian/parser/Def.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <unordered_map>

#include "Netlist.h"
#include "LibraryMapping.h"

namespace ophidian
{
    namespace circuit
    {
        void def2LibraryMapping(
            const parser::Def & def,
            circuit::Netlist & netlist,
            standard_cell::StandardCells & standardCells,
            LibraryMapping & libraryMapping);
    }     // namespace circuit
}     // namespace ophidian


#endif // DEF2LIBRARYMAPPING_H