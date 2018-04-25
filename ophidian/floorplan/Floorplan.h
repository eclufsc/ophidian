/*
 * Copyright 2017 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at
   http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.
 */

#ifndef OPHIDIAN_FLOORPLAN_FLOORPLAN_H
#define OPHIDIAN_FLOORPLAN_FLOORPLAN_H

#include <unordered_map>

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace floorplan
{
    class Row :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class Site :
        public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    /// Floorplan class.

    /**
     * This class provides the basic floorplan interface, such as site and rows insertion, site and rows properties and circuit dimensions.
     */
    class Floorplan
    {
    public:
        using RowsIterator = entity_system::EntitySystem<Row>::const_iterator;
        using SitesIterator = entity_system::EntitySystem<Site>::const_iterator;

        //! Chip origin location setter

        /*!
           \brief Set the chip origin location.
           \param loc Chip origin location.
         */
        void chipOrigin(const util::LocationDbu & loc);

        //! Chip origin location getter

        /*!
           \brief Get the chip origin location.
           \return Chip origin location.
         */
        util::LocationDbu chipOrigin()
        {
            return mChipOrigin;
        }

        //! Chip upper right corner location setter

        /*!
           \brief Set the chip upper right corner location.
           \param loc Chip upper right corner location.
         */
        void chipUpperRightCorner(const util::LocationDbu & loc);

        //! Chip upper right corner location getter

        /*!
           \brief Get the chip upper right corner location.
           \param Chip upper right corner location.
         */
        util::LocationDbu chipUpperRightCorner()
        {
            return mChipUpperRightCorner;
        }

        // sites
        //! Add site in the floorplan

        /*!
           \brief Adds a a new site in the floorplan. A site has a name and a dimension associated to it.
           \param name Name of the site, used to identify it.
           \param dimension LocationDbu describing the site dimension.
           \return The created site.
         */
        Site add(Site, const std::string & name, const util::LocationDbu & loc);

        //! Erase site in the floorplan

        /*!
           \brief Erases an existing site.
           \param site Site to be erased.
         */
        void erase(Site site);


        //! Name getter

        /*!
           \brief Get the name of a given site
           \param site Site entity to get the name.
           \return Name of the site.
         */
        std::string name(const Site & site) const
        {
            return mNames[site];
        }

        //! Site getter

        /*!
           \brief Get the site of a given site name
           \param A name of a site.
           \return Site entity of the given name.
         */
        Site find(std::string siteName)
        {
            return mName2Site[siteName];
        }

        //! Site Upper right corner getter

        /*!
           \brief get the upper right corner of a given site
           \param site Site entity to get the upper right corner.
           \return upper right corner of the site.
         */
        util::LocationDbu siteUpperRightCorner(const Site & site) const
        {
            return mDimensions[site];
        }

        //! Sites iterator

        /*!
           \return Range iterator for the Sites.
         */
        ophidian::util::Range<SitesIterator> sitesRange() const
        {
            return ophidian::util::Range<SitesIterator>(mSites.begin(), mSites.end());
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
        Row add(Row,
            const util::LocationDbu & loc,
            util::database_unit_scalar_t num,
            const Site &site);

        //! Erase row in the floorplan

        /*!
           \brief Erases an existing row.
           \param row Row to be erased.
         */
        void erase(const Row & row);

        //! Origin location getter

        /*!
           \brief Get the origin location of a given row.
           \param row Row entity to get the origin location.
           \return Origin location of the row.
         */
        util::LocationDbu origin(const Row & row) const
        {
            return mOrigins[row];
        }

        //! Number of sites getter

        /*!
           \brief Get the number of sites of a given row.
           \param row Row entity to get the origin location.
           \return Number of sites of the row.
         */
        util::database_unit_scalar_t numberOfSites(const Row & row) const
        {
            return mNumberOfSites[row];
        }

        //! Site type getter

        /*!
           \brief Get the site type which the row is composed of.
           \param row Row entity to get the origin location.
           \return Site type of the row.
         */
        Site site(const Row & row) const
        {
            return mSiteTypeOfRow[row];
        }

        //! Rows iterator

        /*!
           \return Range iterator for the Rows.
         */
        ophidian::util::Range<RowsIterator> rowsRange() const
        {
            return util::Range<RowsIterator>(mRows.begin(), mRows.end());
        }

        /// Row dimensions getter.

        /**
         * Returns the upper right corner of a row, calculated using the number of sites and site dimensions in that row.
         * \param row Row entity to gets the dimensions.
         * \return LocationDbu describing the upper right corner of that row.
         */
        util::LocationDbu rowUpperRightCorner(const Row & row) const;

    private:
        entity_system::EntitySystem<Row>                           mRows{};
        entity_system::Property<Row, util::LocationDbu>            mOrigins{mRows};
        entity_system::Property<Row, util::database_unit_scalar_t> mNumberOfSites{mRows};
        entity_system::Property<Row, Site>                         mSiteTypeOfRow{mRows};

        entity_system::EntitySystem<Site>                mSites{};
        entity_system::Property<Site, std::string>       mNames{mSites};
        entity_system::Property<Site, util::LocationDbu> mDimensions{mSites};

        util::LocationDbu mChipOrigin{util::database_unit_t{0.0}, util::database_unit_t{0.0}};
        util::LocationDbu mChipUpperRightCorner{util::database_unit_t{0.0}, util::database_unit_t{0.0}};

        std::unordered_map<std::string, Site> mName2Site{};
    };
}     //namespace floorplan
}     //namespace ophidian

#endif // OPHIDIAN_FLOORPLAN_FLOORPLAN_H
