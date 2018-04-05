#include "LibraryMapping.h"

namespace ophidian
{
    namespace circuit
    {
        LibraryMapping::LibraryMapping(const Netlist & netlist):
                cells2StdCells_(netlist.makeProperty <standard_cell::Cell>(Cell())),
                pins2StdCells_(netlist.makeProperty <standard_cell::Pin>(Pin()))
        {
        }

        standard_cell::Cell LibraryMapping::cellStdCell(const Cell & cell) const
        {
            return cells2StdCells_[cell];
        }

        void LibraryMapping::cellStdCell(const Cell & cell, const standard_cell::Cell & stdCell)
        {
            cells2StdCells_[cell] = stdCell;
        }

        standard_cell::Pin LibraryMapping::pinStdCell(const Pin & pin) const
        {
            return pins2StdCells_[pin];
        }

        void LibraryMapping::pinStdCell(const Pin & pin, const standard_cell::Pin & stdCell)
        {
            pins2StdCells_[pin] = stdCell;
        }
    }
}