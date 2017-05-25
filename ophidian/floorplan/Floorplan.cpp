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

namespace ophidian
{

namespace floorplan
{

Floorplan::Floorplan()
	: mChipOrigin(0.0, 0.0), mChipUpperRightCorner(0.0, 0.0),
	mOrigins(mRows), mNumberOfSites(mRows), mSiteTypeOfRow(mRows),
	mNames(mSites), mDimensions(mSites)
{

}

Floorplan::~Floorplan()
{

}

void Floorplan::chipOrigin(const util::LocationMicron &loc)
{
	mChipOrigin = loc;
}

void Floorplan::chipUpperRightCorner(const util::LocationMicron &loc)
{
	mChipUpperRightCorner = loc;
}

Site Floorplan::add(Site, const std::string & name, const util::LocationMicron & loc)
{
	auto site = mSites.add();
	mNames[site] = name;
	mDimensions[site] = loc;
	return site;
}

void Floorplan::erase(Site site)
{
	mSites.erase(site);
}

Row Floorplan::add(Row, const util::LocationMicron &loc, size_t num, const Site &site)
{
	auto row = mRows.add();
	mOrigins[row] = loc;
	mNumberOfSites[row] = num;
	mSiteTypeOfRow[row] = site;
	return row;
}

void Floorplan::erase(const Row &row)
{
	mRows.erase(row);
}

util::LocationMicron Floorplan::rowUpperRightCorner(const Row &row) const
{
	auto site = mSiteTypeOfRow[row];
	size_t numSites = mNumberOfSites[row];
	util::LocationMicron uRCorner = mDimensions[site];
	return util::LocationMicron(uRCorner.x() * numSites, uRCorner.y());
}

} //namespace floorplan

} //namespace ophidian
