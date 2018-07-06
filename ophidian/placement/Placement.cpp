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

#include "Placement.h"

namespace ophidian
{
namespace placement
{
    Placement::Placement(const circuit::Netlist & netlist, const Library &library):
            mNetlist(netlist),
            mLibrary(library),
            mCellLocations(netlist.makeProperty<util::LocationDbu>(circuit::CellInstance())),
            mInputLocations(netlist.makeProperty<util::LocationDbu>(circuit::Input())),
            mOutputLocations(netlist.makeProperty<util::LocationDbu>(circuit::Output()))
    {
    }

    Placement::~Placement()
    {
    }

    void Placement::placeCell(const circuit::CellInstance & cell, const util::LocationDbu & location)
    {
        mCellLocations[cell] = location;
    }

    void Placement::placeInputPad(const circuit::Input & input, const util::LocationDbu & location)
    {
        mInputLocations[input] = location;
    }

    util::LocationDbu Placement::inputPadLocation(const circuit::Input & input) const
    {
        return mInputLocations[input];
    }

    void Placement::placeOutputPad(
        const circuit::Output & output,
        const util::LocationDbu & location)
    {
        mOutputLocations[output] = location;
    }

    util::LocationDbu Placement::outputPadLocation(const circuit::Output & output) const
    {
        return mOutputLocations[output];
    }

    geometry::MultiBox<util::database_unit_t> Placement::geometry(const circuit::CellInstance & cell) const
    {
        auto stdCell = mNetlist.std_cell(cell);
        auto stdCellGeometry = mLibrary.geometry(stdCell);
        auto location = cellLocation(cell);

        geometry::MultiBox<util::database_unit_t> cellGeometry = stdCellGeometry.translate(location);

        return cellGeometry;
    }

    util::LocationDbu Placement::location(const circuit::PinInstance & pin) const
    {
        auto stdCellPin = mNetlist.std_cell_pin(pin);
        auto pinOwner = mNetlist.cell(pin);
        auto location = cellLocation(pinOwner);
        auto pinOffset = mLibrary.pinOffset(stdCellPin);

        util::LocationDbu pinLocation(location.x() + pinOffset.x(),
            location.y() + pinOffset.y());

        return pinLocation;
    }
}     //namespace placement
}     //namespace ophidian
