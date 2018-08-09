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

#ifndef OPHIDIAN_PLACEMENT_LIBRARY_H
#define OPHIDIAN_PLACEMENT_LIBRARY_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/geometry/CellGeometry.h>
#include <ophidian/util/Units.h>
#include <ophidian/circuit/StandardCells.h>

namespace ophidian
{
namespace placement
{
    class Library
    {
    public:
        using unit_type = util::database_unit_t;

        using offset_type = util::LocationDbu;

        using std_cell_type = circuit::StandardCells::cell_type;

        using std_cell_pin_type = circuit::StandardCells::pin_type;

        using std_cell_geometry_type = geometry::CellGeometry;

        // Constructors
        Library() = delete;

        Library(const Library&) = delete;
        Library& operator=(const Library&) = delete;

        Library(Library&&) = default;
        Library& operator=(Library&&) = default;

        Library(const circuit::StandardCells & std_cells);

        // Element access
        std_cell_geometry_type& geometry(const std_cell_type& cell);
        const std_cell_geometry_type& geometry(const std_cell_type& cell) const;

        offset_type& offset(const std_cell_pin_type& pin);
        const offset_type& offset(const std_cell_pin_type& pin) const;

        // Iterators

        // Capacity

        // Modifiers
        void connect(const std_cell_type& cell, const std_cell_geometry_type& geometry);

        void connect(const std_cell_pin_type& pin, const offset_type& offset);

    private:
        entity_system::Property<std_cell_type, std_cell_geometry_type> mGeometries;
        entity_system::Property<std_cell_pin_type, offset_type>   mPinOffsets;
    };
}     // namespace placement
}     // namespace ophidian

#endif // LIBRARY_H
