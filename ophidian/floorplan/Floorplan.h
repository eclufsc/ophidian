#ifndef OPHIDIAN_FLOORPLAN_FLOORPLAN_H
#define OPHIDIAN_FLOORPLAN_FLOORPLAN_H

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
public:
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
    void chipOrigin(util::Location loc);

    //! Chip origin location getter
    /*!
       \brief Get the chip origin location.
       \return Chip origin location.
     */
    util::Location chipOrigin()
    {
        return chipOrigin_;
    }

    //! Chip upper right corner location setter
    /*!
       \brief Set the chip upper right corner location.
       \param loc Chip upper right corner location.
     */
    void chipUpperRightCorner(util::Location loc);

    //! Chip upper right corner location getter
    /*!
       \brief Get the chip upper right corner location.
       \param Chip upper right corner location.
     */
    util::Location chipUpperRightCorner()
    {
        return chipUpperRightCorner_;
    }


    // sites
    //! Add site in the floorplan
    /*!
       \brief Adds a a new site in the floorplan. A site has a name and a dimension associated to it.
       \param name Name of the site, used to identify it.
       \param dimension Location describing the site dimension.
       \return The created site.
     */
    Site add(Site, std::string name, util::Location loc);

    //! Erase site in the floorplan
    /*!
       \brief Erases an existing site.
       \param site Site to be erased.
     */
    void erase(Site site);


    //! Name getter
    /*!
       \brief Get the name of a given site
       \param site Site entity to set the name.
       \return Name of the site.
     */
    std::string name(Site site) const
    {
        return sites_.name(site);
    }

    //! Site Upper right corner getter
    /*!
       \brief get the upper right corner of a given site
       \param site Site entity to get the upper right corner.
       \return upper right corner of the site.
     */
    util::Location siteUpperRightCorner(Site site) const
    {
        return sites_.upperRightCorner(site);
    }

    //! Sites iterator
    /*!
       \return Range iterator for the Sites.
     */
    ophidian::util::Range<Sites::SitesIterator> sites_range() const
    {
        return sites_.sites_range();
    }


    // rows
    //! Add row in the floorplan
    /*!
       \brief Adds a a new row in the floorplan. A row has a origin location, number of sites and a siteTyoe associated to it.
       \param loc Origin location of the row.
       \param num Number of sites of the row.
       \param site Site type of the row.
       \return The created row.
     */
    Row add(Row, util::Location loc, size_t num, Site site);

    //! Erase row in the floorplan
    /*!
       \brief Erases an existing row.
       \param row Row to be erased.
     */
    void erase(Row row);

    //! Origin location getter
    /*!
       \brief Get the origin location of a given row.
       \param row Row entity to get the origin location.
       \return Origin location of the row.
     */
    util::Location origin(Row row) const
    {
        return rows_.origin(row);
    }

    //! Number of sites getter
    /*!
       \brief Get the number of sites of a given row.
       \param row Row entity to get the origin location.
       \return Number of sites of the row.
     */
    size_t numberOfSites(Row row) const
    {
        return rows_.numberOfSites(row);
    }

    //! Site type getter
    /*!
       \brief Get the site type which the row is composed of.
       \param row Row entity to get the origin location.
       \return Site type of the row.
     */
    Site site(Row row) const
    {
        return rows_.site(row);
    }

    //! Rows iterator
    /*!
       \return Range iterator for the Rows.
     */
    ophidian::util::Range<Rows::RowsIterator> rows_range() const
    {
        return rows_.rows_range();
    }

    /// Row dimensions getter.
    /**
     * Returns the upper right corner of a row, calculated using the number of sites and site dimensions in that row.
     * \param row Row entity to gets the dimensions.
     * \return Location describing the upper right corner of that row.
     */
    util::Location rowUpperRightCorner(Row row) const;

private:
    Sites sites_;
    Rows rows_;
    util::Location chipOrigin_;
    util::Location chipUpperRightCorner_;
};

} //namespace floorplan

} //namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_FLOORPLAN_H
