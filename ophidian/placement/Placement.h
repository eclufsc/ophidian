#ifndef OPHIDIAN_PLACEMENT_PLACEMENT_H
#define OPHIDIAN_PLACEMENT_PLACEMENT_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>

namespace ophidian {
namespace placement {

class Cell : public entity_system::EntityBase
{
    public:
        using entity_system::EntityBase::EntityBase;
};

class Placement
{
public:
	//! Placement Constructor
	/*!
	   \brief Constructs a placement system with no properties
	 */
	Placement();

    //! Placement Destructor
	/*!
	   \brief Destroys the placement system, including its properties.
	 */
	~Placement();

    //! Add cell in the placement
    /*!
       \brief Adds a a new cell in the placement. A cell has a location.
       \param location Location of the lower left corner of the cell.
       \return The created cell.
     */
    Cell add(Cell, util::Location location);

    //! Erase cell in the placement
    /*!
       \brief Erases an existing cell.
       \param cell Cell to be erased.
     */
    void erase(Cell cell);

    //! Location getter
    /*!
       \brief Get the location of a given cell.
       \param cell Cell entity to get the location.
       \return Location of the cell.
     */
    util::Location location(Cell cell) {
        return locations_[cell];
    }

private:
    entity_system::EntitySystem<Cell> cells_;
    entity_system::Property<Cell, util::Location> locations_;
};

} //namespace placement

} //namespace ophidian

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
