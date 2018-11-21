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
        auto dbuConverter = util::DbuConverter{lef.micrometer_to_dbu_ratio()};

        //creating layers
        for(auto& layer : lef.layers()){
            //layer type
            auto lType = Library::layer_type_type{};

            switch (layer.type()) {
                case ophidian::parser::Lef::layer_type::type_type::CUT:
                    lType = Library::layer_type_type::CUT;
                    break;
                case ophidian::parser::Lef::layer_type::type_type::MASTERSLICE:
                    lType = Library::layer_type_type::MASTERSLICE;
                    break;
                case ophidian::parser::Lef::layer_type::type_type::ROUTING:
                    lType = Library::layer_type_type::ROUTING;
                    break;
                default:
                    lType = Library::layer_type_type::NA;
                    break;
            }

            //layer direction
            auto lDirection = Library::layer_direction_type{};

            switch (layer.direction()) {
                case ophidian::parser::Lef::layer_type::direction_type::HORIZONTAL:
                    lDirection = Library::layer_direction_type::HORIZONTAL;
                    break;
                case ophidian::parser::Lef::layer_type::direction_type::VERTICAL:
                    lDirection = Library::layer_direction_type::VERTICAL;
                    break;
                default:
                    lDirection = Library::layer_direction_type::NA;
                    break;
            }

            //layer spacingTable
            auto lTableContents = Library::spacing_table_content_type{};

            lTableContents.row_values.reserve(layer.parallel_run_length().widths().size());

            for(auto& val : layer.parallel_run_length().widths()){
                lTableContents.row_values.emplace_back(dbuConverter.convert(val));
            }

            lTableContents.column_values.reserve(layer.parallel_run_length().lengths().size());

            for(auto& val : layer.parallel_run_length().lengths()){
                lTableContents.column_values.emplace_back(dbuConverter.convert(val));
            }

            lTableContents.values.reserve(layer.parallel_run_length().lengths().size());

            for(auto& width : layer.parallel_run_length().widths()){
                auto lenghts = std::vector<Library::unit_type>{};

                lenghts.reserve(layer.parallel_run_length().lengths().size());

                for(auto& length : layer.parallel_run_length().lengths()){
                    auto spacing = layer.parallel_run_length().width_length_to_spacing().at({width, length});
                    lenghts.push_back(dbuConverter.convert(spacing));
                }

                lTableContents.values.emplace_back(std::move(lenghts));
            }

            auto lTable = Library::spacing_table_type{lTableContents};

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
                lTable
            );
        }

        //creating vias
        for(auto& via : lef.vias()){
            auto map = Library::layer_name_to_via_geometry_type{};

            for(auto layer_map : via.layers()){
                auto box_micron = layer_map.second.front();

                auto box_dbu = Library::via_geometry_type{
                    dbuConverter.convert(box_micron.min_corner()),
                    dbuConverter.convert(box_micron.max_corner())
                };

                map.emplace(layer_map.first, box_dbu);
            }

            library.add_via(via.name(), map);
        }

        //creating tracks
        for(auto& track : def.tracks()){
            auto orientation = routing::Library::track_orientation_type{};

            if(track.orientation() == parser::Def::track_type::orientation_type::X){
                orientation = Library::track_orientation_type::X;
            }else{
                orientation = Library::track_orientation_type::Y;
            }

            library.add_track(orientation, track.start(), track.number_of_tracks(), track.space(), track.layer_name());
        }
    }
}
