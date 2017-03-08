#ifndef LIBRARY_H
#define LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/util/Units.h>

namespace ophidian {
namespace placement {
class Library
{
public:
    //! Library Constructor
    /*!
       \brief Constructs a placement library.
     */
    Library(const standard_cell::StandardCells & std_cells);

    //! Cell geometry getter
    /*!
       \brief Gets the geometry of a cell.
       \param cell Cell entity to get the geometry.
       \return Geometry of the cell.
     */
    util::MultiBox geometry(const standard_cell::Cell & cell) const {
        return geometries_[cell];
    }

    //! Cell geometry setter
    /*!
       \brief Set the geometry of a cell.
       \param cell Cell entity to set the geometry.
       \param geometry Geometry to assign to cell.
     */
    void geometry(const standard_cell::Cell & cell, const util::MultiBox & geometry);

    //! Pin offset getter
    /*!
       \brief Gets the offset of a pin.
       \param pin Pin entity to get the offset.
       \return Offset of the pin.
     */
    util::Location pinOffset(const standard_cell::Pin & pin) const {
        return pinOffsets_[pin];
    }

    //! Pin offset setter
    /*!
       \brief Sets the offset of a pin.
       \param pin Pin entity to set the offset.
       \param offset Offset to assign to pin.
     */
    void pinOffset(const standard_cell::Pin & pin, const util::Location & offset);

private:
    const standard_cell::StandardCells & std_cells_;

    entity_system::Property<standard_cell::Cell, util::MultiBox> geometries_;
    entity_system::Property<standard_cell::Pin, util::Location> pinOffsets_;
};
}
}

#endif // LIBRARY_H