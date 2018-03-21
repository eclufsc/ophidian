#ifndef PLACEMENTMAPPING_H
#define PLACEMENTMAPPING_H

#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/geometry/Operations.h>

#include "Placement.h"
#include "Library.h"

namespace ophidian
{
namespace placement
{
    class PlacementMapping
    {
    public:

        //! Placement mapping Constructor

        /*!
           \brief Constructs an empty placement mapping from placement and circuit information.
           \param placement Circuit placement
           \param library Placement library
           \param netlist Circuit netlist.
           \param libraryMapping library mapping between the netlist and standard cells library
         */
        PlacementMapping(
            const Placement & placement,
            const Library & library,
            const circuit::Netlist & netlist,
            const circuit::LibraryMapping & libraryMapping);

        //! Cell geometry getter

        /*!
           \brief Get the geometry of a cell in the circuit.
           \param cell Cell entity to get the geometry.
           \return Geometry of the cell.
         */
        geometry::MultiBox<util::database_unit_t> geometry(const circuit::Cell & cell) const;

        //! Pin location getter

        /*!
           \brief Get the location of a pin in the circuit.
           \param cell Pin entity to get the location.
           \return Location of the cell.
         */
        util::LocationDbu location(const circuit::Pin & pin) const;

    private:
        const Placement &               mPlacement;
        const Library &                 mLibrary;
        const circuit::Netlist &        mNetlist;
        const circuit::LibraryMapping & mLibraryMapping;
    };
}
}

#endif // PLACEMENTMAPPING_H
