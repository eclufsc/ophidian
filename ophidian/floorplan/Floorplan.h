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

    class Floorplan
    {
    public:
        using RowsIterator = entity_system::EntitySystem<Row>::const_iterator;
        using SitesIterator = entity_system::EntitySystem<Site>::const_iterator;

        // Constructors
        Floorplan() = default;

        Floorplan(const Floorplan&) = default;
        Floorplan& operator=(const Floorplan&) = default;

        Floorplan(Floorplan&&) = default;
        Floorplan& operator=(Floorplan&&) = default;

        // Element access
        util::LocationDbu origin(const Row & row) const;

        util::LocationDbu& chip_upper_right_corner() noexcept;
        const util::LocationDbu& chip_upper_right_corner() const noexcept;

        util::LocationDbu& chip_origin() noexcept;
        const util::LocationDbu& chip_origin() const noexcept;
        
        std::string& name(const Site & site);
        const std::string& name(const Site & site) const;

        util::LocationDbu& site_upper_right_corner(const Site & site);
        const util::LocationDbu& site_upper_right_corner(const Site & site) const;

        Site find(const std::string& siteName) const;

        util::database_unit_scalar_t& number_of_sites(const Row & row);
        const util::database_unit_scalar_t& number_of_sites(const Row & row) const;

        Site site(const Row & row) const;

        util::LocationDbu row_upper_right_corner(const Row & row) const;

        // Iterators
        ophidian::util::Range<SitesIterator> sitesRange() const
        {
            return ophidian::util::Range<SitesIterator>(mSites.begin(), mSites.end());
        }

        ophidian::util::Range<RowsIterator> rowsRange() const
        {
            return util::Range<RowsIterator>(mRows.begin(), mRows.end());
        }

        // Capacity

        // Modifiers
        void chipOrigin(const util::LocationDbu & loc);

        void chipUpperRightCorner(const util::LocationDbu & loc);

        Site add(Site, const std::string & name, const util::LocationDbu & loc);

        void erase(Site site);

        Row add(Row,
            const util::LocationDbu & loc,
            util::database_unit_scalar_t num,
            const Site &site);

        void erase(const Row & row);

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
}
}

#endif // OPHIDIAN_FLOORPLAN_FLOORPLAN_H
