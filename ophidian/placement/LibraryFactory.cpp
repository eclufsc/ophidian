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

#include "LibraryFactory.h"

namespace ophidian::placement::factory
{
    void make_library(Library& library, const parser::Lef& lef, circuit::StandardCells& stdCells) noexcept
    {
        for(const auto& macro : lef.macros())
        {
            auto stdCell = stdCells.find_cell(macro.name());

            auto pmin = geometry::CellGeometry::point_type{
                macro.origin().x() * lef.micrometer_to_dbu_ratio(),
                macro.origin().y() * lef.micrometer_to_dbu_ratio()
            };
            auto pmax = geometry::CellGeometry::point_type{
                macro.size().x() * lef.micrometer_to_dbu_ratio(),
                macro.size().y() * lef.micrometer_to_dbu_ratio()
            };

            geometry::CellGeometry::box_type box{pmin, pmax};
            library.geometry(stdCell) = geometry::CellGeometry{ {std::make_pair(box, "")} };

            auto dbu_converter = util::DbuConverter{lef.micrometer_to_dbu_ratio()};

            for(const auto& pin : macro.pins())
            {
                auto stdPin = stdCells.find_pin(macro.name() + ":" + pin.name());

                for(const auto& port : pin.ports())
                {
                    for(const auto& rect : port.second)
                    {
                        library.offset(stdPin) = Library::offset_type{
                            0.5 * (dbu_converter.convert(rect.min_corner().x()) + dbu_converter.convert(rect.max_corner().x())),
                            0.5 * (dbu_converter.convert(rect.min_corner().y()) + dbu_converter.convert(rect.max_corner().y())),
                        };
                    }
                }
            }
        }
    }

    void make_library(Library& library, const parser::ICCAD2020 & iccad_2020, circuit::StandardCells& stdCells) noexcept {
        for(const auto& macro : iccad_2020.macros())
        {
            auto stdCell = stdCells.find_cell(macro.name());

            auto pmin = geometry::CellGeometry::point_type{
                macro.origin().x(),
                macro.origin().y()
            };
            auto pmax = geometry::CellGeometry::point_type{
                macro.size().x(),
                macro.size().y()
            };

            geometry::CellGeometry::box_type box{pmin, pmax};
            library.geometry(stdCell) = geometry::CellGeometry{ {std::make_pair(box, "")} };

            for(const auto& pin : macro.pins())
            {
                auto stdPin = stdCells.find_pin(macro.name() + ":" + pin.name());

                library.offset(stdPin) = Library::offset_type{Library::unit_type{0}, Library::unit_type{0}};

                auto min_corner = geometry::CellGeometry::point_type{Library::unit_type{0}, Library::unit_type{0}};
                auto max_corner = geometry::CellGeometry::point_type{Library::unit_type{0}, Library::unit_type{0}};
                library.geometry(stdPin) = Library::std_pin_geometry_type();

                geometry::CellGeometry::box_type box{min_corner, max_corner};
                library.geometry(stdPin).push_back(std::make_pair(box, pin.layer()));
            }
        }
    }
}
