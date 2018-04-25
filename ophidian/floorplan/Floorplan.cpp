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

    util::LocationDbu Floorplan::rowUpperRightCorner(const Row & row) const
    {
        auto site = mSiteTypeOfRow[row];

        util::database_unit_scalar_t numSites = mNumberOfSites[row];

        util::LocationDbu uRCorner = mDimensions[site];

        return util::LocationDbu{uRCorner.x() * numSites, uRCorner.y()};
    }
}     //namespace floorplan
}     //namespace ophidian
