#ifndef LIBRARYMAPPING_H
#define LIBRARYMAPPING_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>

namespace ophidian {
namespace circuit {
class LibraryMapping
{
public:
    LibraryMapping(const Netlist & netlist);

    ophidian::standard_cell::Cell cellStdCell(const Cell & cell) const;

    void cellStdCell(const Cell & cell, const standard_cell::Cell & stdCell);

    ophidian::standard_cell::Pin pinStdCell(const Pin & pin) const;

    void pinStdCell(const Pin & pin, const standard_cell::Pin & stdCell);

private:
    ophidian::entity_system::Property<Cell, standard_cell::Cell> cells2StdCells_;
    ophidian::entity_system::Property<Pin, standard_cell::Pin> pins2StdCells_;
};
}
}


#endif // LIBRARYMAPPING_H
