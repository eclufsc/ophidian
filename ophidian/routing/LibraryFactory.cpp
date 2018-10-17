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
#include <ophidian/util/Units.h>
#include <unordered_map>

namespace ophidian::routing::factory
{
    void make_library(Library& library, const parser::Lef& lef, const parser::Def & def) noexcept
    {
        util::DbuConverter dbuConverter{lef.micrometer_to_dbu_ratio()};

        //creating layers
        for(auto& layer : lef.layers()){
            //layer type
            LayerType lType;

            switch (layer.type()) {
                case ophidian::parser::Lef::layer_type::Type::CUT:
                    lType = LayerType::CUT;
                    break;
                case ophidian::parser::Lef::layer_type::Type::MASTERSLICE:
                    lType = LayerType::MASTERSLICE;
                    break;
                case ophidian::parser::Lef::layer_type::Type::ROUTING:
                    lType = LayerType::ROUTING;
                    break;
                default:
                    lType = LayerType::NA;
                    break;
            }

            //layer direction
            LayerDirection lDirection;
            switch (layer.direction()) {
                case ophidian::parser::Lef::layer_type::Direction::HORIZONTAL:
                lDirection = LayerDirection::HORIZONTAL;
                break;
            case ophidian::parser::Lef::layer_type::Direction::VERTICAL:
                lDirection = LayerDirection::VERTICAL;
                break;
            default:
                lDirection = LayerDirection::NA;
                break;
            }
            //layer spacingTable
            Library::spacing_table_content_type lTableContents;
            lTableContents.row_values.reserve(layer.parallel_run_length().widths().size());
            for(auto& val : layer.parallel_run_length().widths()){
                lTableContents.row_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.column_values.reserve(layer.parallel_run_length().lengths().size());
            for(auto& val : layer.parallel_run_length().lengths()){
                lTableContents.column_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.values.reserve(layer.parallel_run_length().lengths().size());
            for(auto& width : layer.parallel_run_length().widths()){
                std::vector<Library::unit_type> v;
                v.reserve(layer.parallel_run_length().lengths().size());
                for(auto& length : layer.parallel_run_length().lengths()){
                    auto spacing = layer.parallel_run_length().width_length_to_spacing().at({width, length});
                    v.push_back(dbuConverter.convert(spacing));
                }
                lTableContents.values.push_back(v);
            }
            auto lTable = Library::spacing_table_type(lTableContents);
            auto l = library.add_layer(
                        layer.name(),
                        lType,
                        lDirection,
                        dbuConverter.convert(layer.pitch()),
                        dbuConverter.convert(layer.offset()),
                        dbuConverter.convert(layer.width()),
                        dbuConverter.convert(layer.min_width()),
                        dbuConverter.convert(layer.area()),
                        dbuConverter.convert(layer.spacing()),
                        dbuConverter.convert(layer.end_of_line().space()),
                        dbuConverter.convert(layer.end_of_line().width()),
                        dbuConverter.convert(layer.end_of_line().within()),
                        lTable);
        }

        //creating vias
         for(auto& via : lef.vias()){
             auto map = Library::layer_name_to_via_geometry_type{};
             for(auto layer_map : via.layers()){
                 auto box_micron = layer_map.second.front();
                 auto box_dbu = Library::via_geometry_type{dbuConverter.convert(box_micron.min_corner()), dbuConverter.convert(box_micron.max_corner())};
                 map.emplace(layer_map.first, box_dbu);
             }
             library.add_via(via.name(), map);
         }

        //creating tracks
         for(auto track : def.tracks()){
            ophidian::routing::TrackOrientation orientation;
            if(track.orientation() == ophidian::parser::Def::track_type::Orientation::X){
                orientation = ophidian::routing::TrackOrientation::X;
            }else {
                orientation = ophidian::routing::TrackOrientation::Y;
            }
            library.add_track(orientation, track.start(), track.number_of_tracks(), track.space(), track.layer_name());
         }
    }
}
