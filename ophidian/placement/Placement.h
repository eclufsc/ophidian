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

#ifndef OPHIDIAN_PLACEMENT_PLACEMENT_H
#define OPHIDIAN_PLACEMENT_PLACEMENT_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/util/Range.h>
#include <ophidian/util/Units.h>
#include <ophidian/circuit/Netlist.h>

namespace ophidian
{
namespace placement
{

class Placement
{
public:
	//! Placement Constructor
	/*!
	   \brief Constructs a placement system with no properties
	 */
	Placement(const circuit::Netlist & netlist);

	//! Placement Destructor
	/*!
	   \brief Destroys the placement system, including its properties.
	 */
	~Placement();

	//! Places a cell
	/*!
	   \brief Places a cell by setting its location
	   \param cell Cell to be placed
       \param location LocationDbu of the lower left corner of the cell.
	 */
    void placeCell(const circuit::Cell & cell, const util::LocationDbu & location);

    //! LocationDbu getter
	/*!
	   \brief Get the location of a given cell.
	   \param cell Cell entity to get the location.
       \return LocationDbu of the cell.
	 */
    util::LocationDbu location(const circuit::Cell & cell) const {
		return mLocations[cell];
	}

private:
    entity_system::Property<circuit::Cell, util::LocationDbu> mLocations;
};

} //namespace placement

} //namespace ophidian

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
