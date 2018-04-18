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

#ifndef OPHIDIAN_DESIGN_DESIGN_H
#define OPHIDIAN_DESIGN_DESIGN_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/floorplan/Floorplan.h>
#include <ophidian/placement/Placement.h>
#include <ophidian/placement/Library.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/placement/PlacementMapping.h>
#include <ophidian/standard_cell/StandardCells.h>

namespace ophidian
{
	/// Design describing a whole system
	namespace design
	{
		class Design
		{
		public:

			//! Design Constructor

			/*!
			   \brief Constructs a design system with no properties
			*/
			Design();

			//! Design Destructor

			/*!
			   \brief Destroys the design system, including its properties.
			*/
			~Design();

			//! netlist getter

			/*!
			   \brief Get the netlist.
			   \return Netlist.
			*/
			circuit::Netlist & netlist()
			{
				return mNetlist;
			}

			//! floorplan getter

			/*!
			   \brief Get the floorplan.
			   \return Floorplan.
			*/
			floorplan::Floorplan & floorplan()
			{
				return mFloorplan;
			}

			//! placement getter

			/*!
			   \brief Get the placement.
			   \return Placement.
			*/
			placement::Placement & placement()
			{
				return mPlacement;
			}

			//! standardCells getter

			/*!
			   \brief Get standardCells.
			   \return StandardCells.
			*/
			standard_cell::StandardCells & standardCells()
			{
				return mStandardCells;
			}

			//! library getter

			/*!
			   \brief Get the library.
			   \return Library.
			*/
			placement::Library & library()
			{
				return mLibrary;
			}

			//! libraryMapping getter

			/*!
			   \brief Get the libraryMapping.
			   \return LibraryMapping.
			*/
			circuit::LibraryMapping & libraryMapping()
			{
				return mLibraryMapping;
			}

			//! placementMapping getter

			/*!
			   \brief Get the placementMapping.
			   \return PlacementMapping.
			*/
			placement::PlacementMapping & placementMapping()
			{
				return mPlacementMapping;
			}

		private:
			circuit::Netlist             mNetlist;
			floorplan::Floorplan         mFloorplan;
			placement::Placement         mPlacement;
			standard_cell::StandardCells mStandardCells;
			placement::Library           mLibrary;
			circuit::LibraryMapping      mLibraryMapping;
			placement::PlacementMapping  mPlacementMapping;

		};
	} //namespace design
} //namespace ophidian

#endif // OPHIDIAN_DESIGN_DESIGN_H
