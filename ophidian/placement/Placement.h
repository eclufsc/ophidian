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

        using box_type = geometry::Box<unit_type>;

        using cell_type = circuit::Netlist::cell_instance_type;

        using pin_type = circuit::Netlist::pin_instance_type;

        using pad_type = circuit::Netlist::pad_type;

        using cell_geometry_type = geometry::CellGeometry;

        using pin_geometry_type = geometry::CellGeometry;

        using pad_geometry_type = geometry::CellGeometry;

        using fixed_type = bool;

        using cell_container_type = std::vector<cell_type>;

        using unitless_point_type       = geometry::Point<double>;
        using unitless_box_type       = geometry::Box<double>;
        using rtree_node_type       = std::pair<unitless_point_type, cell_type>;
        using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16> >;


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

        const point_type& location(const pad_type& pad) const;

        cell_geometry_type geometry(const cell_type& cell) const;

        pin_geometry_type geometry(const pin_type& pin) const;

        pad_geometry_type geometry(const pad_type& pad) const;

        orientation_type orientation(const cell_type& cell) const;

        orientation_type orientation(const pad_type& pad) const;

        const fixed_type isFixed(const cell_type& cell) const;

        void cells_within(const box_type & region, cell_container_type & cells) const;
        // Iterators

        // Capacity

        // Modifiers
        void place(const cell_type& cell, const point_type& location, bool update_rtree = false);

        void place(const pad_type& pad, const point_type & location);

        void setOrientation(const Placement::cell_type& cell, const orientation_type& orientation);

        void setOrientation(const pad_type& pad, const orientation_type& orientation);

        void add_geometry(const pad_type& pad, const pad_geometry_type::box_type& box, const pad_geometry_type::layer_name_type& layer_name);

        void fixLocation(const cell_type& cell);

        void unfixLocation(const cell_type& cell);

        void reset_rtree();
     
    private:
        const circuit::Netlist & m_netlist;
        const Library & m_library;

        entity_system::Property<cell_type, point_type> m_cell_locations;
        entity_system::Property<cell_type, fixed_type> m_fixed_cells;
        entity_system::Property<cell_type, orientation_type> m_cell_orientation;

        entity_system::Property<pad_type, point_type> m_pad_locations;
        entity_system::Property<pad_type, pad_geometry_type> m_pad_geometry;
        entity_system::Property<pad_type, orientation_type> m_pad_orientation;

        rtree_type m_cells_rtree;
    };
}

#endif // OPHIDIAN_PLACEMENT_PLACEMENT_H
