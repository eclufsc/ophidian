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
#include <ophidian/geometry/Operations.h>
#include <ophidian/geometry/Models.h>

namespace ophidian::placement
{
    Placement::Placement(const circuit::Netlist & netlist, const Library &library):
            m_netlist(netlist),
            m_library(library),
            m_cell_locations(netlist.make_property_cell_instance<util::LocationDbu>()),
            m_cell_orientation(netlist.make_property_cell_instance<Placement::orientation_type>()),
            m_input_pad_locations(netlist.make_property_input_pad<util::LocationDbu>()),
            m_output_pad_locations(netlist.make_property_output_pad<util::LocationDbu>()),
            m_fixed_cells(netlist.make_property_cell_instance<fixed_type>())
    {
    }

    // Element access
    const Placement::point_type& Placement::location(const Placement::cell_type& cell) const
    {
        return m_cell_locations[cell];
    }

    Placement::point_type Placement::location(const Placement::pin_type& pin) const
    {
        auto pinInput = m_netlist.input(pin);
        if (pinInput != input_pad_type()) {
            return location(pinInput);
        }

        auto pinOutput = m_netlist.output(pin);
        if (pinOutput != output_pad_type()) {
            return location(pinOutput);
        }

        auto stdCellPin = m_netlist.std_cell_pin(pin);
        auto pinOwner = m_netlist.cell(pin);
        auto cell_location = location(pinOwner);
        auto pinOffset = m_library.offset(stdCellPin);

        auto pin_location = Placement::point_type{
            cell_location.x() + pinOffset.x(),
            cell_location.y() + pinOffset.y()
        };

        return pin_location;
    }

    const Placement::point_type& Placement::location(const Placement::input_pad_type& input) const
    {
        return m_input_pad_locations[input];
    }

    const Placement::point_type& Placement::location(const Placement::output_pad_type& output) const
    {
        return m_output_pad_locations[output];
    }

    Placement::cell_geometry_type Placement::geometry(const Placement::cell_type& cell) const
    {
        auto stdCell = m_netlist.std_cell(cell);
        auto stdCellGeometry = m_library.geometry(stdCell);
        auto cell_location = location(cell);

        auto cellGeometry = geometry::translate(stdCellGeometry, cell_location);

        return cellGeometry;
    }

    Placement::pin_geometry_type Placement::geometry(const Placement::pin_type& pin) const
    {
        using Point = ophidian::geometry::Point<double>;
        using Box = ophidian::geometry::Box<double>;


        auto stdPin = m_netlist.std_cell_pin(pin);
        auto stdPinGeometry = m_library.geometry(stdPin);
        auto cell = m_netlist.cell(pin);
        auto std_cell = m_netlist.std_cell(cell);

        auto cell_location = location(cell);
        auto cell_width = m_library.geometry(std_cell).width();
        auto cell_height = m_library.geometry(std_cell).height();

        auto translated_boxes = geometry::CellGeometry{};
        translated_boxes.reserve(stdPinGeometry.size());

        for(auto & geometry : stdPinGeometry)
        {
            point_type min_corner;
            point_type max_corner;
            auto& box = geometry.first;

            switch (orientation(cell)){
                case orientation_type::N:
                    min_corner = point_type{
                        box.min_corner().x() + cell_location.x(),
                        box.min_corner().y() + cell_location.y()
                    };
                    max_corner = point_type{
                        box.max_corner().x() + cell_location.x(),
                        box.max_corner().y() + cell_location.y()
                    };
                    break;
                case orientation_type::FN:
                    min_corner = point_type{
                        cell_location.x() + cell_width - box.max_corner().x(),
                        box.min_corner().y() + cell_location.y()
                    };
                    max_corner = point_type{
                        cell_location.x() + cell_width - box.min_corner().x(),
                        box.max_corner().y() + cell_location.y()
                    };
                    break;
                case orientation_type::S:
                    min_corner = point_type{
                        cell_location.x() + cell_width - box.max_corner().x(),
                        cell_location.y() + cell_height - box.max_corner().y()
                    };
                    max_corner = point_type{
                        cell_location.x() + cell_width - box.min_corner().x(),
                        cell_location.y() + cell_height - box.min_corner().y()
                    };
                    break;
                case orientation_type::FS:
                    min_corner = point_type{
                        box.min_corner().x() + cell_location.x(),
                        cell_location.y() + cell_height - box.max_corner().y()
                    };
                    max_corner = point_type{
                        box.max_corner().x() + cell_location.x(),
                        cell_location.y() + cell_height - box.min_corner().y()
                    };
                    break;
            }
            geometry::CellGeometry::box_type new_box (
                min_corner, max_corner
            );
            translated_boxes.push_back(std::make_pair(new_box, geometry.second));
        }
        return translated_boxes;
    }

    Placement::orientation_type Placement::orientation(const Placement::cell_type& cell) const
    {
        return m_cell_orientation[cell];
    }


    const Placement::fixed_type Placement::isFixed(const Placement::cell_type& cell) const{
        return m_fixed_cells[cell];
    }

    // Modifiers
    void Placement::place(const Placement::cell_type& cell, const Placement::point_type& location)
    {
        m_cell_locations[cell] = location;
    }

    void Placement::place(const Placement::input_pad_type& input, const Placement::point_type & location)
    {
        m_input_pad_locations[input] = location;
    }

    void Placement::place(const Placement::output_pad_type& output, const Placement::point_type & location)
    {
        m_output_pad_locations[output] = location;
    }

    void Placement::setOrientation(const Placement::cell_type& cell, const orientation_type& orientation)
    {
        m_cell_orientation[cell] = orientation;
    }

    void Placement::fixLocation(const Placement::cell_type& cell)
    {
        m_fixed_cells[cell] = true;
    }

    void Placement::unfixLocation(const Placement::cell_type& cell)
    {
        m_fixed_cells[cell] = false;
    }
}
