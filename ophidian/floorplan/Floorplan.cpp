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
    util::LocationDbu Floorplan::origin(const Row & row) const
    {
        return mOrigins[row];
    }

    util::LocationDbu& Floorplan::chip_upper_right_corner() noexcept
    {
        return mChipUpperRightCorner;
    }

    const util::LocationDbu& Floorplan::chip_upper_right_corner() const noexcept
    {
        return mChipUpperRightCorner;
    }

    util::LocationDbu& Floorplan::chip_origin() noexcept
    {
        return mChipOrigin;
    }

    const util::LocationDbu& Floorplan::chip_origin() const noexcept
    {
        return mChipOrigin;
    }

    std::string& Floorplan::name(const Site & site)
    {
        return mNames[site];
    }

    const std::string& Floorplan::name(const Site & site) const
    {
        return mNames[site];
    }

    util::LocationDbu& Floorplan::site_upper_right_corner(const Site & site)
    {
        return mDimensions[site];
    }

    const util::LocationDbu& Floorplan::site_upper_right_corner(const Site & site) const
    {
        return mDimensions[site];
    }

    Site Floorplan::find(const std::string& siteName) const
    {
        return mName2Site.at(siteName);
    }

    util::database_unit_scalar_t& Floorplan::number_of_sites(const Row & row)
    {
        return mNumberOfSites[row];
    }

    const util::database_unit_scalar_t& Floorplan::number_of_sites(const Row & row) const
    {
        return mNumberOfSites[row];
    }

    Site Floorplan::site(const Row & row) const
    {
        return mSiteTypeOfRow[row];
    }

    util::LocationDbu Floorplan::row_upper_right_corner(const Row & row) const
    {
        auto site = mSiteTypeOfRow[row];

        util::database_unit_scalar_t numSites = mNumberOfSites[row];

        util::LocationDbu uRCorner = mDimensions[site];

        return util::LocationDbu{uRCorner.x() * numSites, uRCorner.y()};
    }

    // Iterators
    void Floorplan::chipOrigin(const util::LocationDbu & loc)
    {
        mChipOrigin = loc;
    }

    void Floorplan::chipUpperRightCorner(const util::LocationDbu & loc)
    {
        mChipUpperRightCorner = loc;
    }

    Site Floorplan::add(Site, const std::string & name, const util::LocationDbu & loc)
    {
        auto site = mSites.add();

        mNames[site] = name;
        mName2Site[name] = site;
        mDimensions[site] = loc;

        return site;
    }

    void Floorplan::erase(Site site)
    {
        mName2Site.erase(name(site));
        mSites.erase(site);
    }

    Row Floorplan::add(
        Row,
        const util::LocationDbu & loc,
        util::database_unit_scalar_t num,
        const Site & site)
    {
        auto row = mRows.add();

        mOrigins[row] = loc;
        mNumberOfSites[row] = num;
        mSiteTypeOfRow[row] = site;

        return row;
    }

    void Floorplan::erase(const Row & row)
    {
        mRows.erase(row);
    }

}     //namespace floorplan
}     //namespace ophidian
