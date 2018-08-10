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

namespace ophidian::floorplan
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

        using point_type = util::LocationDbu;

        using site_type = Site;
        using site_name_type = std::string;

        using row_type = Row;
        using row_size_type = util::database_unit_scalar_t;

        // Constructors
        Floorplan() = default;

        Floorplan(const Floorplan&) = default;
        Floorplan& operator=(const Floorplan&) = default;

        Floorplan(Floorplan&&) = default;
        Floorplan& operator=(Floorplan&&) = default;

        // Element access
        point_type& chip_origin() noexcept;
        const point_type& chip_origin() const noexcept;
        
        point_type& chip_upper_right_corner() noexcept;
        const point_type& chip_upper_right_corner() const noexcept;

        point_type& origin(const row_type & row);
        const point_type& origin(const row_type & row) const;

        point_type upper_right_corner(const row_type & row) const;

        row_size_type& number_of_sites(const row_type & row);
        const row_size_type& number_of_sites(const row_type & row) const;

        const site_type& site(const row_type & row) const;

        site_name_type& name(const site_type & site);
        const site_name_type& name(const site_type & site) const;

        point_type& dimension(const site_type & site);
        const point_type& dimension(const site_type & site) const;

        const site_type& find(const site_name_type& siteName) const;

        // Iterators
        ophidian::util::Range<SitesIterator> range_site() const;

        ophidian::util::Range<RowsIterator> range_row() const;

        // Capacity

        // Modifiers
        const site_type& add_site(const site_name_type & name, const point_type & loc);

        const row_type& add_row(const point_type & loc, const row_size_type& num, const site_type &site);

        void erase(const site_type& site);

        void erase(const row_type & row);

    private:
        entity_system::EntitySystem<row_type>                           mRows{};
        entity_system::Property<row_type, point_type>            mOrigins{mRows};
        entity_system::Property<row_type, util::database_unit_scalar_t> mNumberOfSites{mRows};
        entity_system::Property<row_type, site_type>                         mSiteTypeOfRow{mRows};

        entity_system::EntitySystem<site_type>                mSites{};
        entity_system::Property<site_type, site_name_type>       mNames{mSites};
        entity_system::Property<site_type, point_type> mDimensions{mSites};

        point_type mChipOrigin{util::database_unit_t{0.0}, util::database_unit_t{0.0}};
        point_type mChipUpperRightCorner{util::database_unit_t{0.0}, util::database_unit_t{0.0}};

        std::unordered_map<site_name_type, site_type> mName2Site{};
    };
}

#endif // OPHIDIAN_FLOORPLAN_FLOORPLAN_H
