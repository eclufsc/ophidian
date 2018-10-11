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
#include <unordered_map>

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
            lTableContents.row_values.reserve(layer.parallelRunLength().numWidth());
            for(auto& val : layer.parallelRunLength().widths()){
                lTableContents.row_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.column_values.reserve(layer.parallelRunLength().numLength());
            for(auto& val : layer.parallelRunLength().lengths()){
                lTableContents.column_values.push_back(dbuConverter.convert(val));
            }
            lTableContents.values.reserve(layer.parallelRunLength().numLength());
            for(auto& width : layer.parallelRunLength().widths()){
                std::vector<Library::unit_type> v;
                v.reserve(layer.parallelRunLength().numLength());
                for(auto& length : layer.parallelRunLength().lengths()){
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
         for(auto& via : lef.vias()){
             auto map = Library::via_layer_map_type{};
             for(auto layer_map : via.layers()){
                 auto box_micron = layer_map.second.front();
                 auto box_dbu = Library::box_type{dbuConverter.convert(box_micron.min_corner()), dbuConverter.convert(box_micron.max_corner())};
                 map.emplace(layer_map.first, box_dbu);
             }
             library.add_via_instance(via.name(), map);
         }

        //creating tracks
         for(auto track : def.tracks()){
            ophidian::routing::TrackOrientation orientation;
            if(track.orientation() == ophidian::parser::Def::Track::Orientation::X){
                orientation = ophidian::routing::TrackOrientation::X;
            }else {
                orientation = ophidian::routing::TrackOrientation::Y;
            }
            library.add_track_instance(orientation, track.start(), track.number_of_tracks(), track.space(), track.layer_name());
         }
    }
}
}
}
