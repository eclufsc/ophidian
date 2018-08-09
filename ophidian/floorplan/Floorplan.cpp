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

#include "Floorplan.h"
#include "iostream"

namespace ophidian
{
namespace floorplan
{
    // Element access
    Floorplan::point_type& Floorplan::chip_origin() noexcept
    {
        return mChipOrigin;
    }

    const Floorplan::point_type& Floorplan::chip_origin() const noexcept
    {
        return mChipOrigin;
    }

    Floorplan::point_type& Floorplan::chip_upper_right_corner() noexcept
    {
        return mChipUpperRightCorner;
    }

    const Floorplan::point_type& Floorplan::chip_upper_right_corner() const noexcept
    {
        return mChipUpperRightCorner;
    }

    Floorplan::point_type& Floorplan::origin(const Floorplan::row_type & row)
    {
        return mOrigins[row];
    }

    const Floorplan::point_type& Floorplan::origin(const Floorplan::row_type & row) const
    {
        return mOrigins[row];
    }

    Floorplan::point_type Floorplan::upper_right_corner(const Floorplan::row_type & row) const
    {
        auto site = mSiteTypeOfRow[row];

        util::database_unit_scalar_t numSites = mNumberOfSites[row];

        Floorplan::point_type uRCorner = mDimensions[site];

        return Floorplan::point_type{uRCorner.x() * numSites + origin(row).x(), uRCorner.y() + origin(row).y()};
    }

    Floorplan::site_name_type& Floorplan::name(const Floorplan::site_type & site)
    {
        return mNames[site];
    }

    const Floorplan::site_name_type& Floorplan::name(const Floorplan::site_type & site) const
    {
        return mNames[site];
    }

    Floorplan::point_type& Floorplan::dimension(const Floorplan::site_type & site)
    {
        return mDimensions[site];
    }

    const Floorplan::point_type& Floorplan::dimension(const Floorplan::site_type & site) const
    {
        return mDimensions[site];
    }

    const Floorplan::site_type& Floorplan::find(const Floorplan::site_name_type& siteName) const
    {
        return mName2Site.at(siteName);
    }

    Floorplan::row_size_type& Floorplan::number_of_sites(const Floorplan::row_type & row)
    {
        return mNumberOfSites[row];
    }

    const Floorplan::row_size_type& Floorplan::number_of_sites(const Floorplan::row_type & row) const
    {
        return mNumberOfSites[row];
    }

    const Floorplan::site_type& Floorplan::site(const Floorplan::row_type & row) const
    {
        return mSiteTypeOfRow[row];
    }

    // Iterators
    ophidian::util::Range<Floorplan::SitesIterator> Floorplan::range_site() const
    {
        return ophidian::util::Range<Floorplan::SitesIterator>(mSites.begin(), mSites.end());
    }
    
    ophidian::util::Range<Floorplan::RowsIterator> Floorplan::range_row() const
    {
        return util::Range<Floorplan::RowsIterator>(mRows.begin(), mRows.end());
    }

    // Modifiers
    const Floorplan::site_type& Floorplan::add_site(const Floorplan::site_name_type & name, const Floorplan::point_type & loc)
    {
        const auto& site = mSites.add();

        mNames[site] = name;
        mName2Site[name] = site;
        mDimensions[site] = loc;

        return site;
    }

    const Floorplan::row_type& Floorplan::add_row(
        const Floorplan::point_type & loc,
        const Floorplan::row_size_type& num,
        const Floorplan::site_type & site)
    {
        const auto& row = mRows.add();

        mOrigins[row] = loc;
        mNumberOfSites[row] = num;
        mSiteTypeOfRow[row] = site;

        return row;
    }

    void Floorplan::erase(const Floorplan::site_type& site)
    {
        mName2Site.erase(name(site));
        mSites.erase(site);
    }

    void Floorplan::erase(const Floorplan::row_type & row)
    {
        mRows.erase(row);
    }

}     //namespace floorplan
}     //namespace ophidian
