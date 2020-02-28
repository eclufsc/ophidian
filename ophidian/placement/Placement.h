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
#include <ophidian/geometry/CellGeometry.h>

namespace ophidian::placement
{
    class Placement
    {
    public:
        using unit_type = util::database_unit_t;

        using point_type = util::LocationDbu;

        using cell_type = circuit::Netlist::cell_instance_type;

        using pin_type = circuit::Netlist::pin_instance_type;

        using input_pad_type = circuit::Netlist::input_pad_type;

        using output_pad_type = circuit::Netlist::output_pad_type;

        using cell_geometry_type = geometry::CellGeometry;

        using pin_geometry_type = geometry::CellGeometry;

        using fixed_type = bool;

        // Class member types
        enum class Orientation : int {
            N, S, W, E, FN, FS, FW, FE
        };

        using orientation_type = Orientation;

        // Constructors
        Placement() = delete;

        Placement(const Placement&) = delete;
        Placement& operator=(const Placement&) = delete;

        Placement(Placement&&) = delete;
        Placement& operator=(Placement&&) = delete;

        Placement(const circuit::Netlist & netlist, const Library & library);

        // Element access
        const point_type& location(const cell_type& cell) const;

        point_type location(const pin_type& pin) const;

        const point_type& location(const input_pad_type& input) const;

        const point_type& location(const output_pad_type& output) const;

        cell_geometry_type geometry(const cell_type& cell) const;

        pin_geometry_type geometry(const pin_type& pin) const;

        orientation_type orientation(const cell_type& cell) const;

        const fixed_type isFixed(const cell_type& cell) const;
        // Iterators

        // Capacity

        // Modifiers
        void place(const cell_type& cell, const point_type& location, const orientation_type& orientation);

        void place(const input_pad_type& input, const point_type & location);

        void place(const output_pad_type& output, const point_type & location);

        void fixLocation(const cell_type& cell);

        void unfixLocation(const cell_type& cell);

    private:
        const circuit::Netlist & m_netlist;
        const Library & m_library;

        entity_system::Property<cell_type, point_type> m_cell_locations;
        entity_system::Property<cell_type, fixed_type> m_fixed_cells;
        entity_system::Property<cell_type, orientation_type> m_cell_orientation;
        entity_system::Property<input_pad_type, point_type> m_input_pad_locations;
        entity_system::Property<output_pad_type, point_type> m_output_pad_locations;

    };
}

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
