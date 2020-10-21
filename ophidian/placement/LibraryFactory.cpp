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
            //std::cout << macro.name() << std::endl;

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
                //std::cout << pin.name() << std::endl;

                auto stdPin = stdCells.find_pin(macro.name() + ":" + pin.name());

                library.geometry(stdPin) = Library::std_pin_geometry_type();

                for(const auto& port : pin.ports())
                {
                    auto layer_name = port.first;
                    
                    geometry::CellGeometry::unit_type sum_x{0};
                    geometry::CellGeometry::unit_type sum_y{0};
                    double cont = 0.0;

                    for(const auto& rect : port.second)
                    {
                        // library.offset(stdPin) = Library::offset_type{
                        //     0.5 * (dbu_converter.convert(rect.min_corner().x()) + dbu_converter.convert(rect.max_corner().x())),
                        //     0.5 * (dbu_converter.convert(rect.min_corner().y()) + dbu_converter.convert(rect.max_corner().y())),
                        // };

                        auto min_x = dbu_converter.convert(std::min( rect.min_corner().x(), rect.max_corner().x() ));
                        auto min_y = dbu_converter.convert(std::min( rect.min_corner().y(), rect.max_corner().y() ));
                        auto max_x = dbu_converter.convert(std::max( rect.min_corner().x(), rect.max_corner().x() ));
                        auto max_y = dbu_converter.convert(std::max( rect.min_corner().y(), rect.max_corner().y() ));

                        sum_x += (min_x + max_x)/2;
                        sum_y += (min_y + max_y)/2;
                        cont ++;

                        auto min_corner = geometry::CellGeometry::point_type{ min_x, min_y };
                        auto max_corner = geometry::CellGeometry::point_type{ max_x, max_y };
                        geometry::CellGeometry::box_type box{min_corner, max_corner};

                        library.geometry(stdPin).push_back(std::make_pair(box, layer_name));

                        //std::cout << "box " << min_x.value() << "," << min_y.value() << "->" << max_x.value() << "," << max_y.value() << std::endl;
                    }

                    /*double center_of_mass_x = sum_x.value() / cont;
                    double center_of_mass_y = sum_y.value() / cont;
                    library.offset(stdPin) = Library::offset_type{ 
                        geometry::CellGeometry::unit_type(center_of_mass_x), 
                        geometry::CellGeometry::unit_type(center_of_mass_y) 
                    };*/

                    library.offset(stdPin) = Library::offset_type{
                        geometry::CellGeometry::unit_type{port.second[0].min_corner().x()},
                        geometry::CellGeometry::unit_type{port.second[0].min_corner().y()}
                    };
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
