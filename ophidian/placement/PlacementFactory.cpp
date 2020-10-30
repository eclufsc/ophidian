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

#include "PlacementFactory.h"

namespace ophidian::placement::factory
{
    void make_placement(Placement& placement, const parser::Def & def, circuit::Netlist& netlist) noexcept
    {
        for(const auto & component : def.components())
        {
            auto cell = netlist.find_cell_instance(component.name());
            Placement::Orientation orientation;
            switch (component.orientation())
            {
                case ophidian::parser::Component::Orientation::N :
                    orientation = Placement::Orientation::N;
                    break;
                case ophidian::parser::Component::Orientation::S :
                    orientation = Placement::Orientation::S;
                    break;
                case ophidian::parser::Component::Orientation::W :
                    orientation = Placement::Orientation::W;
                    break;
                case ophidian::parser::Component::Orientation::E :
                    orientation = Placement::Orientation::E;
                    break;
                case ophidian::parser::Component::Orientation::FN :
                    orientation = Placement::Orientation::FN;
                    break;
                case ophidian::parser::Component::Orientation::FS :
                    orientation = Placement::Orientation::FS;
                    break;
                case ophidian::parser::Component::Orientation::FW :
                    orientation = Placement::Orientation::FW;
                    break;
                case ophidian::parser::Component::Orientation::FE :
                    orientation = Placement::Orientation::FE;
                    break;
                default:
                    orientation = Placement::Orientation::N;
            }
            placement.place(cell, component.position());
            placement.setOrientation(cell, orientation);
            if(component.fixed())
                placement.fixLocation(cell);
            else
                placement.unfixLocation(cell);
        }

        for(const auto & pad : def.pads())
        {
            auto name = pad.name();
            // auto direction = pad.direction();
            auto pin_instance = netlist.add_pin_instance("PIN:" + name);
            auto pad_instance = netlist.add_pad(pin_instance);

            auto pad_position = pad.position();

            for(auto layer_map : pad.layers())
            {
                auto layer_name = layer_map.first;
                for(auto rect : layer_map.second)
                {
                    auto min_x = std::min( rect.min_corner().x(), rect.max_corner().x());
                    auto min_y = std::min( rect.min_corner().y(), rect.max_corner().y());
                    auto max_x = std::max( rect.min_corner().x(), rect.max_corner().x());
                    auto max_y = std::max( rect.min_corner().y(), rect.max_corner().y());

                    auto min_corner = Placement::pad_geometry_type::point_type{ pad_position.x() + min_x, pad_position.y() + min_y };
                    auto max_corner = Placement::pad_geometry_type::point_type{ pad_position.x() + max_x, pad_position.y() + max_y };
                    Placement::pad_geometry_type::box_type box{min_corner, max_corner};

                    placement.add_geometry(pad_instance, box, layer_name);
                }
            }
        }
    }

    void make_placement(Placement& placement, const parser::ICCAD2020 & iccad_2020, const circuit::Netlist& netlist) noexcept {
        auto gcell_size = 10;
        for(const auto & component : iccad_2020.components())
        {
            auto cell = netlist.find_cell_instance(component.name());
            Placement::Orientation orientation = Placement::Orientation::N;
            auto cell_location = component.position();
            auto adjusted_location = Placement::point_type{Placement::unit_type{(cell_location.x().value()-1)*gcell_size + gcell_size / 2},
                                                           Placement::unit_type{(cell_location.y().value()-1)*gcell_size + gcell_size / 2}};
            placement.place(cell, adjusted_location);
            placement.setOrientation(cell, orientation);
            if(component.fixed())
                placement.fixLocation(cell);
            else
                placement.unfixLocation(cell);
        }
    }
}
