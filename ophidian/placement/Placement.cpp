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

    // Element access
    const Placement::point_type& Placement::location(const Placement::cell_type& cell) const
    {
        return mCellLocations[cell];
    }

    Placement::point_type Placement::location(const Placement::pin_type& pin) const
    {
        auto stdCellPin = mNetlist.std_cell_pin(pin);
        auto pinOwner = mNetlist.cell(pin);
        auto cell_location = location(pinOwner);
        auto pinOffset = mLibrary.offset(stdCellPin);

        auto pin_location = Placement::point_type{
            cell_location.x() + pinOffset.x(),
            cell_location.y() + pinOffset.y()
        };

        return pin_location;
    }

    const Placement::point_type& Placement::location(const Placement::input_pad_type& input) const
    {
        return mInputLocations[input];
    }

    const Placement::point_type& Placement::location(const Placement::output_pad_type& output) const
    {
        return mOutputLocations[output];
    }


    Placement::cell_geometry_type Placement::geometry(const Placement::cell_type& cell) const
    {
        auto stdCell = mNetlist.std_cell(cell);
        auto stdCellGeometry = mLibrary.geometry(stdCell);
        auto cell_location = location(cell);

        auto cellGeometry = geometry::translate(stdCellGeometry, cell_location);

        return cellGeometry;
    }

    // Modifiers
    void Placement::place(const Placement::cell_type& cell, const Placement::point_type& location)
    {
        mCellLocations[cell] = location;
    }

    void Placement::place(const Placement::input_pad_type& input, const Placement::point_type & location)
    {
        mInputLocations[input] = location;
    }

    void Placement::place(const Placement::output_pad_type& output, const Placement::point_type & location)
    {
        mOutputLocations[output] = location;
    }
}     //namespace placement
}     //namespace ophidian
