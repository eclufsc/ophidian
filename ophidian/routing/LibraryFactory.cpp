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
#include "ophidian/util/Units.h"

namespace ophidian
{
namespace routing
{
namespace factory
{
    void make_library(Library& library, const parser::Lef& lef, const parser::Def & def) noexcept
    {
        util::DbuConverter dbuConverter{lef.micrometer_to_dbu_ratio()};

        //creating layers
        for(auto& layer : lef.layers()){
            //layer type
            LayerType lType;
            switch (layer.type()) {
            case ophidian::parser::Lef::Layer::Type::CUT:
                lType = LayerType::CUT;
                break;
            case ophidian::parser::Lef::Layer::Type::MASTERSLICE:
                lType = LayerType::MASTERSLICE;
                break;
            case ophidian::parser::Lef::Layer::Type::ROUTING:
                lType = LayerType::ROUTING;
                break;
            default:
                lType = LayerType::NA;
                break;
            }
            //layer direction
            LayerDirection lDirection;
            switch (layer.direction()) {
            case ophidian::parser::Lef::Layer::Direction::HORIZONTAL:
                lDirection = LayerDirection::HORIZONTAL;
                break;
            case ophidian::parser::Lef::Layer::Direction::VERTICAL:
                lDirection = LayerDirection::VERTICAL;
                break;
            default:
                lDirection = LayerDirection::NA;
                break;
            }
            //layer spacingTable
            Library::spacing_table_content_type lTableContents;
            lTableContents.row_values.reserve(layer.parallelRunLength().numWidth());
            for(auto& val : layer.parallelRunLength().widths()){
                lTableContents.row_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.column_values.reserve(layer.parallelRunLength().numLength());
            for(auto& val : layer.parallelRunLength().lengths()){
                lTableContents.column_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.values.reserve(layer.parallelRunLength().numLength());
            for(auto& length : layer.parallelRunLength().lengths()){
                std::vector<Library::unit_type> v;
                v.reserve(layer.parallelRunLength().numWidth());
                for(auto& width : layer.parallelRunLength().widths()){
                    auto spacing = layer.parallelRunLength().spacing(width, length);
                    v.push_back(dbuConverter.convert(spacing));
                }
                lTableContents.values.push_back(v);
            }            
            auto lTable = Library::spacing_table_type(lTableContents);
            auto l = library.add_layer_instance(
                        layer.name(),
                        lType,
                        lDirection,
                        dbuConverter.convert(layer.pitch()),
                        dbuConverter.convert(layer.offset()),
                        dbuConverter.convert(layer.width()),
                        dbuConverter.convert(layer.min_width()),
                        dbuConverter.convert(layer.area()),
                        dbuConverter.convert(layer.spacing()),
                        dbuConverter.convert(layer.EOL().space()),
                        dbuConverter.convert(layer.EOL().width()),
                        dbuConverter.convert(layer.EOL().within()),
                        lTable);
        }


        //creating vias





//        for(auto& macro : lef.macros())
//        {
//            auto stdCell = stdCells.add_cell(macro.name());
//            auto layer2RectsM1 = macro.obstructions().find("metal1");
//            if(layer2RectsM1 != macro.obstructions().end()) {
//                auto geometry = geometry::CellGeometry{};
//                for(auto& rect : layer2RectsM1->second)
//                {
//                    geometry::Point<util::database_unit_t> pmin =
//                    {rect.min_corner().x() * lef.micrometer_to_dbu_ratio(),
//                     rect.min_corner().y() * lef.micrometer_to_dbu_ratio()};
//                    geometry::Point<util::database_unit_t> pmax =
//                    {rect.max_corner().x() * lef.micrometer_to_dbu_ratio(),
//                     rect.max_corner().y() * lef.micrometer_to_dbu_ratio()};
//                    geometry.push_back(geometry::Box<util::database_unit_t>(pmin, pmax));
//                }
//                library.connect(stdCell, geometry);
//            }
//            else {
//                geometry::Point<util::database_unit_t> pmin =
//                    {
//                        macro.origin().x() * lef.micrometer_to_dbu_ratio(),
//                        macro.origin().y() * lef.micrometer_to_dbu_ratio()
//                    };
//                geometry::Point<util::database_unit_t> pmax =
//                    {
//                        macro.size().x() * lef.micrometer_to_dbu_ratio(),
//                        macro.size().y() * lef.micrometer_to_dbu_ratio()
//                    };
//                library.connect(stdCell,
//                    geometry::CellGeometry{std::vector<geometry::Box<util::database_unit_t>>{geometry::Box<util::database_unit_t>{pmin, pmax}}});
//            }
//            util::DbuConverter dbuConverter{lef.micrometer_to_dbu_ratio()};

//            for(auto& pin : macro.pins())
//            {
//                auto stdPin = stdCells.add_pin(
//                    macro.name() + ":" + pin.name(),
//                    circuit::PinDirection(pin.direction()));
//                stdCells.connect(stdCell, stdPin);
//                for(auto& port : pin.ports())
//                {
//                    for(auto& rect : port.second)
//                    {
//                        library.connect(
//                            stdPin,
//                            util::LocationDbu(0.5 *
//                                (dbuConverter.convert(rect.min_corner().x()) +
//                                 dbuConverter.convert(rect.max_corner().x())),
//                                0.5 *
//                                (dbuConverter.convert(rect.min_corner().y()) +
//                                 dbuConverter.convert(rect.max_corner().y()))));
//                    }
//                }
//            }
//        }
    }
}
}
}
