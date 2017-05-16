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

namespace ophidian {

namespace floorplan {

Floorplan::Floorplan()
    : chipOrigin_(0.0, 0.0), chipUpperRightCorner_(0.0, 0.0),
      origins_(rows_), numberOfSites_(rows_), siteTypeOfRow_(rows_),
      names_(sites_), dimensions_(sites_)
{

}

Floorplan::~Floorplan()
{

}

void Floorplan::chipOrigin(const util::Location &loc)
{
    chipOrigin_ = loc;
}

void Floorplan::chipUpperRightCorner(const util::Location &loc)
{
    chipUpperRightCorner_ = loc;
}

Site Floorplan::add(Site, const std::string & name, const util::Location & loc)
{
    auto site = sites_.add();
    names_[site] = name;
    name2Site_[name] = site;
    dimensions_[site] = loc;
    return site;
}

void Floorplan::erase(Site site)
{
    name2Site_.erase(name(site));
    sites_.erase(site);
}

Row Floorplan::add(Row, const util::Location &loc, size_t num, const Site &site)
{
    auto row = rows_.add();
    origins_[row] = loc;
    numberOfSites_[row] = num;
    siteTypeOfRow_[row] = site;
    return row;
}

void Floorplan::erase(const Row &row)
{
    rows_.erase(row);
}

util::Location Floorplan::rowUpperRightCorner(const Row &row) const
{
    auto site = siteTypeOfRow_[row];
    size_t numSites = numberOfSites_[row];
    util::Location uRCorner = dimensions_[site];
    return util::Location(uRCorner.x() * numSites, uRCorner.y());
}

} //namespace floorplan

} //namespace ophidian
