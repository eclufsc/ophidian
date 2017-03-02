#ifndef OPHIDIAN_PLACEMENT_PLACEMENT_H
#define OPHIDIAN_PLACEMENT_PLACEMENT_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>
#include <ophidian/circuit/Netlist.h>

namespace ophidian {
namespace placement {

class Placement
{
public:
	//! Placement Constructor
	/*!
	   \brief Constructs a placement system with no properties
	 */
    Placement(const circuit::Netlist & netlist);

    //! Placement Destructor
	/*!
	   \brief Destroys the placement system, including its properties.
	 */
	~Placement();

    //! Places a cell
    /*!
       \brief Places a cell by setting its location
       \param cell Cell to be placed
       \param location Location of the lower left corner of the cell.
     */
    void placeCell(const circuit::Cell & cell, const util::Location & location);

    //! Location getter
    /*!
       \brief Get the location of a given cell.
       \param cell Cell entity to get the location.
       \return Location of the cell.
     */
    util::Location location(const circuit::Cell & cell) const {
        return locations_[cell];
    }

private:
    const circuit::Netlist & netlist_;
    entity_system::Property<circuit::Cell, util::Location> locations_;
};

} //namespace placement

} //namespace ophidian

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
