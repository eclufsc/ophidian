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
            m_fixed_cells(netlist.make_property_cell_instance<fixed_type>()),
            m_pad_locations(netlist.make_property_pad<util::LocationDbu>()),
            m_pad_geometry(netlist.make_property_pad<Placement::pad_geometry_type>()),
            m_pad_orientation(netlist.make_property_pad<Placement::orientation_type>())
    {
    }

    // Element access
    const Placement::point_type& Placement::location(const Placement::cell_type& cell) const
    {
        return m_cell_locations[cell];
    }

    Placement::point_type Placement::location(const Placement::pin_type& pin) const
    {
        if (m_netlist.is_pad(pin)) {
            auto pad = m_netlist.pad(pin);
            return m_pad_locations[pad];
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

    const Placement::point_type& Placement::location(const Placement::pad_type& pad) const
    {
        return m_pad_locations[pad];
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

        //std::cout << "geometry of pin " << m_netlist.name(pin) << std::endl;

        if (m_netlist.is_pad(pin)) {
            auto pad = m_netlist.pad(pin);
            return geometry(pad);
        }

        auto stdPin = m_netlist.std_cell_pin(pin);
        auto stdPinGeometry = m_library.geometry(stdPin);
        auto cell = m_netlist.cell(pin);
        auto std_cell = m_netlist.std_cell(cell);

        auto cell_location = location(cell);
        auto cell_width = m_library.geometry(std_cell).width();
        auto cell_height = m_library.geometry(std_cell).height();

        auto translated_boxes = geometry::CellGeometry{};
        translated_boxes.reserve(stdPinGeometry.size());

        //std::cout << "cell location " << cell_location.x().value() << "," << cell_location.y().value() << std::endl;
        //std::cout << "cell dimensions " << cell_width.value() << "," << cell_height.value() << std::endl;

        for(auto & geometry : stdPinGeometry)
        {
            point_type min_corner;
            point_type max_corner;
            auto& box = geometry.first;

            //std::cout << "pin box " << box.min_corner().x().value() << "," << box.min_corner().y().value() << "->" << box.max_corner().x().value() << "," << box.max_corner().y().value() << std::endl;
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
                case orientation_type::W: // R90
                    min_corner = point_type{
                        cell_location.x() + cell_height - box.max_corner().y(),
                        cell_location.y() + box.min_corner().x()
                    };
                    max_corner = point_type{
                        cell_location.x() + cell_height - box.min_corner().y(),
                        cell_location.y() + box.max_corner().x()
                    };
                    break;
                
                case orientation_type::FW: // MX90
                    min_corner = point_type{
                        cell_location.x() + box.min_corner().y(),
                        cell_location.y() + box.min_corner().x()
                    };
                    max_corner = point_type{
                        cell_location.x() + box.max_corner().y(),
                        cell_location.y() + box.max_corner().x()
                    };
                    break;
                case orientation_type::E: // R270
                    min_corner = point_type{
                        cell_location.x() + box.min_corner().y(),
                        cell_location.y() + cell_width - box.max_corner().x()
                    };
                    max_corner = point_type{
                        cell_location.x() + box.max_corner().y(),
                        cell_location.y() + cell_width - box.min_corner().x()
                    };
                    break;
                case orientation_type::FE: // MY90
                    min_corner = point_type{
                        cell_location.x() + cell_height - box.max_corner().y(),
                        cell_location.y() + cell_width - box.max_corner().x()
                    };
                    max_corner = point_type{
                        cell_location.x() + cell_height - box.min_corner().y(),
                        cell_location.y() + cell_width - box.min_corner().x()
                    };
                    break;
            }
            geometry::CellGeometry::box_type new_box (
                min_corner, max_corner
            );
            translated_boxes.push_back(std::make_pair(new_box, geometry.second));
            
            //std::cout << "translated pin box " << new_box.min_corner().x().value() << "," << new_box.min_corner().y().value() << "->" << new_box.max_corner().x().value() << "," << new_box.max_corner().y().value() << std::endl;
        }
        return translated_boxes;
    }

    Placement::pad_geometry_type Placement::geometry(const Placement::pad_type& pad) const
    {
        return m_pad_geometry[pad];
    }

    Placement::orientation_type Placement::orientation(const Placement::cell_type& cell) const
    {
        return m_cell_orientation[cell];
    }

    Placement::orientation_type Placement::orientation(const Placement::pad_type& pad) const
    {
        return m_pad_orientation[pad];
    }

    const Placement::fixed_type Placement::isFixed(const Placement::cell_type& cell) const{
        return m_fixed_cells[cell];
    }

    void Placement::cells_within(const box_type & region, cell_container_type & cells) const {
        auto min_corner = unitless_point_type{region.min_corner().x().value(), region.min_corner().y().value()};
        auto max_corner = unitless_point_type{region.max_corner().x().value(), region.max_corner().y().value()};
        auto box = unitless_box_type{min_corner, max_corner};

        cells.clear();
        std::vector<rtree_node_type> nodes;
        m_cells_rtree.query(boost::geometry::index::within(box), std::back_inserter(nodes));
        cells.reserve(nodes.size());
        for (auto node : nodes) {
            cells.push_back(node.second);
        }
    }

    // Modifiers
    void Placement::place(const Placement::cell_type& cell, const Placement::point_type& location, bool update_rtree)
    {
        auto previous_location = m_cell_locations[cell];
        m_cell_locations[cell] = location;
        if (update_rtree) {
            auto previous_node = rtree_node_type{unitless_point_type{previous_location.x().value(), previous_location.y().value()}, cell};
            m_cells_rtree.remove(previous_node);
            auto new_node = rtree_node_type{unitless_point_type{location.x().value(), location.y().value()}, cell};
            m_cells_rtree.insert(new_node);
        }
    }

    void Placement::place(const Placement::pad_type& pad, const Placement::point_type & location)
    {
        m_pad_locations[pad] = location;
    }

    void Placement::setOrientation(const Placement::cell_type& cell, const orientation_type& orientation)
    {
        m_cell_orientation[cell] = orientation;
    }

    void Placement::setOrientation(const Placement::pad_type& pad, const Placement::orientation_type& orientation)
    {
        m_pad_orientation[pad] = orientation;
    }

    void Placement::add_geometry(const Placement::pad_type& pad, const Placement::pad_geometry_type::box_type& box, const Placement::pad_geometry_type::layer_name_type& layer_name)
    {
        m_pad_geometry[pad].push_back(std::make_pair(box, layer_name));
    }

    void Placement::fixLocation(const Placement::cell_type& cell)
    {
        m_fixed_cells[cell] = true;
    }

    void Placement::unfixLocation(const Placement::cell_type& cell)
    {
        m_fixed_cells[cell] = false;
    }

    void Placement::reset_rtree()
    {
        m_cells_rtree.clear();
        for (auto cell_it = m_netlist.begin_cell_instance(); cell_it != m_netlist.end_cell_instance(); cell_it++) {
            auto cell = *cell_it;
            auto cell_location = m_cell_locations[cell];
            auto node = rtree_node_type{unitless_point_type{cell_location.x().value(), cell_location.y().value()}, cell};
            m_cells_rtree.insert(node);
        }
    }

}
