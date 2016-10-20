#ifndef OPHIDIAN_FLOORPLAN_FLOORPLAN_H
#define OPHIDIAN_FLOORPLAN_FLOORPLAN_H

#include <ophidian/geometry/Models.h>
#include "Sites.h"
#include "Rows.h"

namespace ophidian {

/// Namespace describing floorplan entities, properties and basic floorplan interface.
namespace floorplan {
/// Floorplan class.
/**
 * This class provides the basic floorplan interface, such as site and rows insertion, site and rows properties and circuit dimensions.
 */
class Floorplan
{
    //! Floorplan Constructor
    /*!
       \brief Constructs a floorplan system with no properties
     */
    Floorplan();

    //! Floorplan Destructor
    /*!
       \brief Destroys the floorplan system, including its properties.
     */
    ~Floorplan();

    //! Chip origin location setter
    /*!
       \brief Set the chip origin location.
       \param loc Chip origin location.
     */
    void chipOrigin(geometry::Location loc);

    //! Chip upper right corner location setter
    /*!
       \brief Set the chip upper right corner location.
       \param loc Chip upper right corner location.
     */
    void chipUpperRightCorner(geometry::Location loc);


    // sites
    //! Add site in the floorplan
    /*!
       \brief Adds a a new site in the floorplan. A site has a name and a dimension associated to it.
       \param name Name of the site, used to identify it.
       \param dimension Location describing the site dimension.
       \return The created site.
     */
    Site add(Site, std::string name, geometry::Location dimension);

    //! Erase site in the floorplan
    /*!
       \brief Erases an existing site.
       \param site Site to be erased.
     */
    void erase(Site site);


    // rows
    //! Add row in the floorplan
    /*!
       \brief Adds a a new row in the floorplan. A row has a origin location, number of sites and a siteTyoe associated to it.
       \param loc Origin location of the row.
       \param num Number of sites of the row.
       \param site Site type of the row.
       \return The created row.
     */
    Site add(Row, geometry::Location loc, size_t num, Site site);

    //! Erase row in the floorplan
    /*!
       \brief Erases an existing row.
       \param row Row to be erased.
     */
    void erase(Row row);

private:
    Sites sites_;
    Rows rows_;
    geometry::Location chipOrigin_;
    geometry::Location chipUpperRightCorner_;
};

} //namespace floorplan

} //namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_FLOORPLAN_H
