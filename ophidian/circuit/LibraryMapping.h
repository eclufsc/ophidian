#ifndef LIBRARYMAPPING_H
#define LIBRARYMAPPING_H

#include <ophidian/standard_cell/StandardCells.h>

#include "Netlist.h"

namespace ophidian
{
namespace circuit
{
    class LibraryMapping
    {
    public:

        //! Library mapping Constructor

        /*!
           \brief Constructs an empty library mapping from a netlist.
           \param netlist Circuit netlist.
         */
        LibraryMapping(const Netlist & netlist);

        //! Cell standard cell getter

        /*!
           \brief Get the standard cell of a given cell.
           \param cell Cell entity to get the standard cell.
           \return Standard cell of the cell.
         */
        ophidian::standard_cell::Cell cellStdCell(const Cell & cell) const;

        //! Sets cell standard cell

        /*!
           \brief Sets the standard cell of a cell.
           \param cell Cell entity to be set.
           \param stdCell Standard cell of the cell.
         */
        void cellStdCell(const Cell & cell, const standard_cell::Cell & stdCell);

        //! Pin standard cell getter

        /*!
           \brief Get the standard cell of a given pin.
           \param pin Pin entity to get the standard cell.
           \return Standard cell of the pin.
         */
        ophidian::standard_cell::Pin pinStdCell(const Pin & pin) const;

        //! Sets pin standard cell

        /*!
           \brief Sets the standard cell of a pin.
           \param pin Pin entity to be set.
           \param stdCell Standard cell of the pin.
         */
        void pinStdCell(const Pin & pin, const standard_cell::Pin & stdCell);

    private:
        ophidian::entity_system::Property<Cell, standard_cell::Cell> cells2StdCells_;
        ophidian::entity_system::Property<Pin, standard_cell::Pin>   pins2StdCells_;
    };
}
}


#endif // LIBRARYMAPPING_H