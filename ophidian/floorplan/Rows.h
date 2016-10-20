#ifndef OPHIDIAN_FLOORPLAN_ROWS_H
#define OPHIDIAN_FLOORPLAN_ROWS_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/geometry/Models.h>
#include <ophidian/util/Range.h>

#include "Sites.h"

namespace ophidian {
namespace floorplan {

class Row : public entity_system::EntityBase
{
public:
    using entity_system::EntityBase::EntityBase;
};

/*! An entity system of rows */
class Rows
{
public:
    using RowsIterator = entity_system::EntitySystem<Row>::const_iterator;

    //! Rows Constructor
    /*!
      \brief Constructs an empty system with no Rows
    */
    Rows();

    //! Rows Destructor
    /*!
      \brief Destroys the Row EntitySystem, including its properties.
    */
    ~Rows();

    //! Add Row
    /*!
      \brief Adds a Row instance.
      \return A handler for the created Row.
    */
    Row add();

    //! Erase Row
    /*!
      \brief erases a Row instance.
      \param row Row entity to erase.
    */
    void erase(Row row);

    //! Origin location setter
    /*!
      \brief Set the origin location of a given row.
      \param row Row entity to set the name.
      \param loc Origin location of the row.
    */
    void origin(Row row, geometry::Location loc);

    //! Number of sites setter
    /*!
      \brief Set the number of sites of a given row.
      \param row Row entity to set the name.
      \param num Number of sites of the row.
    */
    void numberOfSites(Row row, size_t num);

    //! Site type setter
    /*!
      \brief Set which site type the row is composed of.
      \param row Row entity to set the name.
      \param site Site type of the row.
    */
    void site(Row row, Site site);

    //! Origin location getter
    /*!
      \brief Get the origin location of a given row.
      \param row Row entity to get the origin location.
      \return Origin location of the row.
    */
    geometry::Location origin(Row row) const;

    //! Number of sites getter
    /*!
      \brief Get the number of sites of a given row.
      \param row Row entity to get the origin location.
      \return Number of sites of the row.
    */
    size_t numberOfSites(Row row) const;

    //! Site type getter
    /*!
      \brief Get the site type which the row is composed of.
      \param row Row entity to get the origin location.
      \return Site type of the row.
    */
    Site site(Row row) const;

    //! Rows iterator
    /*!
      \return Range iterator for the Rows.
    */
    ophidian::util::Range<RowsIterator> rows_range() const;

private:
    entity_system::EntitySystem<Row> rows_;
    entity_system::Property<Row, geometry::Location> origins_;
    entity_system::Property<Row, size_t> numberOfSites_;
    entity_system::Property<Row, Site> siteTypeOfRow_;

};

} //namespace floorplan

} //namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_ROWS_H
