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
#include <ophidian/placement/Library.h>
#include <ophidian/geometry/MultiBox.h>

namespace ophidian
{
namespace placement
{
    class Placement
    {
    public:

        Placement(const circuit::Netlist & netlist, const Library & library);

        ~Placement();

        void placeCell(const circuit::CellInstance & cell, const util::LocationDbu & location);

        util::LocationDbu cellLocation(const circuit::CellInstance & cell) const
        {
            return mCellLocations[cell];
        }

        void placeInputPad(const circuit::Input & input, const util::LocationDbu & location);

        util::LocationDbu inputPadLocation(const circuit::Input & input) const;

        void placeOutputPad(const circuit::Output & output, const util::LocationDbu & location);

        util::LocationDbu outputPadLocation(const circuit::Output & output) const;

        geometry::MultiBox<util::database_unit_t> geometry(const circuit::CellInstance & cell) const;

        util::LocationDbu location(const circuit::PinInstance &pin) const;

    private:
        const circuit::Netlist & mNetlist;
        const Library & mLibrary;

        entity_system::Property<circuit::CellInstance, util::LocationDbu>   mCellLocations;
        entity_system::Property<circuit::Input, util::LocationDbu>  mInputLocations;
        entity_system::Property<circuit::Output, util::LocationDbu> mOutputLocations;
    };
}     //namespace placement
}     //namespace ophidian

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
