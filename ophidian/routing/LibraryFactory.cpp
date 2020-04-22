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
#include <boost/lexical_cast.hpp>
#include <ophidian/util/Units.h>
#include <limits>
#include <unordered_map>
#include <regex>

namespace ophidian::routing::factory
{
    void make_library(Library& library, const parser::Lef& lef, const parser::Def & def) noexcept
    {
        auto dbuConverter = util::DbuConverter{lef.micrometer_to_dbu_ratio()};

        //creating layers
        int highest_layer_index = -1;
        for(auto& layer : lef.layers()){
            //layer type
            auto lType = LayerType{};

            switch (layer.type()) {
                case ophidian::parser::Lef::layer_type::type_type::CUT:
                    lType = LayerType::CUT;
                    break;
                case ophidian::parser::Lef::layer_type::type_type::MASTERSLICE:
                    lType = LayerType::MASTERSLICE;
                    break;
                case ophidian::parser::Lef::layer_type::type_type::ROUTING:
                    lType = LayerType::ROUTING;
                    break;
                default:
                    lType = LayerType::NA;
                    break;
            }

            //layer direction
            auto lDirection = Direction{};

            switch (layer.direction()) {
                case ophidian::parser::Lef::layer_type::direction_type::HORIZONTAL:
                    lDirection = Direction::HORIZONTAL;
                    break;
                case ophidian::parser::Lef::layer_type::direction_type::VERTICAL:
                    lDirection = Direction::VERTICAL;
                    break;
                default:
                    lDirection = Direction::NA;
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
            auto lTable = Library::spacing_table_type(lTableContents);

            auto l_name = layer.name();
            int l_index = -1;
            std::regex word_regex("(\\d+)");
            auto words_begin = std::sregex_iterator(l_name.begin(), l_name.end(), word_regex);
            auto words_end = std::sregex_iterator();
            if(words_begin != words_end)
            {
                std::string indexLayerSegment = words_begin->str();
                l_index = boost::lexical_cast<int>(indexLayerSegment);
            }

            auto l = library.add_layer_instance(
                        layer.name(),
                        l_index,
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
                        dbuConverter.convert(layer.parallel_edge().space()),
                        dbuConverter.convert(layer.parallel_edge().width()),
                        dbuConverter.convert(layer.parallel_edge().within()),
                        dbuConverter.convert(layer.parallel_edge().par_space()),
                        dbuConverter.convert(layer.parallel_edge().par_within()),
                        lTable,
                        dbuConverter.convert(layer.adjacent_cut_spacing().adj_spacing()),
                        layer.adjacent_cut_spacing().cuts(),
                        dbuConverter.convert(layer.adjacent_cut_spacing().cut_within_length()),
                        dbuConverter.convert(layer.corner_spacing().eol_width()));

            if(l_index >= highest_layer_index){
                highest_layer_index = l_index;
                library.set_highest_layer(l);
            }
        }

        //creating vias
        for(auto& via : lef.vias())
        {
            Library::via_geometries_container_type layers{};
            for(auto layer_map : via.layers())
            {
                auto layer_geometry = ophidian::routing::geometry_in_layer_type();
                layer_geometry.layer = library.find_layer_instance(layer_map.first);
                
                for(auto box_micron : layer_map.second)
                {
                    auto box_dbu = Library::box_type{dbuConverter.convert(box_micron.min_corner()), dbuConverter.convert(box_micron.max_corner())};
                    layer_geometry.boxes.push_back(std::move(box_dbu));
                }
                layers.push_back(std::move(layer_geometry));
            }     
            library.add_via_instance(via.name(), layers);
        }

        //creating tracks
         for(auto track : def.tracks()){
            ophidian::routing::Direction orientation;
            if(track.orientation() == ophidian::parser::Def::track_type::Orientation::HORIZONTAL){
                orientation = ophidian::routing::Direction::HORIZONTAL;
            }else {
                orientation = ophidian::routing::Direction::VERTICAL;
            }
            library.add_track_instance(orientation, track.start(), track.number_of_tracks(), track.space(), track.layer_name());
         }

        //pads
        for(auto pad : def.pads())
        {
            auto position = Library::point_type{ pad.position().x(), pad.position().y() };
            Library::orientation_type orientation;
            switch (pad.orientation())
            {
            case ophidian::parser::Pad::Orientation::S:
                orientation = ophidian::routing::Orientation::S;
                break;
            case ophidian::parser::Pad::Orientation::W:
                orientation = ophidian::routing::Orientation::W;
                break;
            case ophidian::parser::Pad::Orientation::E:
                orientation = ophidian::routing::Orientation::E;
                break;
            case ophidian::parser::Pad::Orientation::FN:
                orientation = ophidian::routing::Orientation::FN;
                break;
            case ophidian::parser::Pad::Orientation::FS:
                orientation = ophidian::routing::Orientation::FS;
                break;
            case ophidian::parser::Pad::Orientation::FW:
                orientation = ophidian::routing::Orientation::FW;
                break;
            case ophidian::parser::Pad::Orientation::FE:
                orientation = ophidian::routing::Orientation::FE;
                break;
            default:
                orientation = ophidian::routing::Orientation::N;
                break;
            }

            Library::pad_geometries_container_type layers{};
            for(auto layer_map : pad.layers())
            {
                auto layer_geometry = ophidian::routing::geometry_in_layer_type();
                layer_geometry.layer = library.find_layer_instance(layer_map.first);
                for(auto box : layer_map.second)
                {
                    Library::point_type min_corner;
                    Library::point_type max_corner;
                    auto box_width = std::abs(units::unit_cast<double>(box.max_corner().x()) - units::unit_cast<double>(box.min_corner().x()));
                    auto box_height = std::abs(units::unit_cast<double>(box.max_corner().y()) - units::unit_cast<double>(box.min_corner().y()));
                    
                    switch (orientation)
                    {
                    case ophidian::routing::Orientation::S:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{0.5 * box_width},
                            position.y() - Library::unit_type{box_height}
                        };
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{0.5 * box_width},
                            position.y()};
                        break;
                    case ophidian::routing::Orientation::N:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{0.5 * box_width},
                            position.y()};
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{0.5 * box_width},
                            position.y() + Library::unit_type{box_height}};
                        break;
                    case ophidian::routing::Orientation::W:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{box_height},
                            position.y() - Library::unit_type{0.5 * box_width}};
                        max_corner = Library::point_type{
                            position.x(),
                            position.y() + Library::unit_type{0.5 * box_width}};
                        break;
                    case ophidian::routing::Orientation::E:
                        min_corner = Library::point_type{
                            position.x(),
                            position.y() - Library::unit_type{0.5 * box_width}};
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{box_height},
                            position.y() + Library::unit_type{0.5 * box_width}};
                        break;
                    case ophidian::routing::Orientation::FN:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{0.5 * box_width},
                            position.y()};
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{0.5 * box_width},
                            position.y() + Library::unit_type{box_height}};
                        break;
                    case ophidian::routing::Orientation::FS:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{0.5 * box_width},
                            position.y() - Library::unit_type{box_height}};
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{0.5 * box_width},
                            position.y()};
                        break;
                    case ophidian::routing::Orientation::FE:
                        min_corner = Library::point_type{
                            position.x() - Library::unit_type{box_height},
                            position.y() - Library::unit_type{0.5 * box_width}};
                        max_corner = Library::point_type{
                            position.x(),
                            position.y() + Library::unit_type{0.5 * box_width}};
                        break;
                    case ophidian::routing::Orientation::FW:
                        min_corner = Library::point_type{
                            position.x(),
                            position.y() - Library::unit_type{0.5 * box_width}};
                        max_corner = Library::point_type{
                            position.x() + Library::unit_type{box_height},
                            position.y() + Library::unit_type{0.5 * box_width}};
                        break;
                    }
                    auto box_dbu = Library::box_type{ min_corner, max_corner};
                    layer_geometry.boxes.emplace_back(std::move(box_dbu));
                }
                layers.push_back(std::move(layer_geometry));
            }
            library.add_pad_instance(pad.name(), position, orientation, layers);
        }
    }

    void make_library(Library& library, const ophidian::circuit::StandardCells& std_cells, const parser::ICCAD2020 & iccad_2020) noexcept {
        util::DbuConverter dbuConverter{1};

        int highest_index = std::numeric_limits<int>::min();
        ophidian::routing::Layer highest_layer;
        for(auto& layer : iccad_2020.layers()){
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
            Direction lDirection;
            switch (layer.direction()) {
                case ophidian::parser::Lef::layer_type::Direction::HORIZONTAL:
                lDirection = Direction::HORIZONTAL;
                break;
            case ophidian::parser::Lef::layer_type::Direction::VERTICAL:
                lDirection = Direction::VERTICAL;
                break;
            default:
                lDirection = Direction::NA;
                break;
            }
            //layer spacingTable
            Library::spacing_table_content_type lTableContents;
            auto lTable = Library::spacing_table_type(lTableContents);
            auto l = library.add_layer_instance(
                        layer.name(),
                        layer.index(),
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
                        dbuConverter.convert(layer.parallel_edge().space()),
                        dbuConverter.convert(layer.parallel_edge().width()),
                        dbuConverter.convert(layer.parallel_edge().within()),
                        dbuConverter.convert(layer.parallel_edge().par_space()),
                        dbuConverter.convert(layer.parallel_edge().par_within()),
                        lTable,
                        dbuConverter.convert(layer.adjacent_cut_spacing().adj_spacing()),
                        layer.adjacent_cut_spacing().cuts(),
                        dbuConverter.convert(layer.adjacent_cut_spacing().cut_within_length()),
                        dbuConverter.convert(layer.corner_spacing().eol_width()));
            if(layer.index() >= highest_index)
            {
                highest_index = layer.index();
                highest_layer = l;
            }
        }
        library.set_highest_layer(highest_layer);
        for(auto std_cell : std_cells.range_cell())
        {
            auto std_cell_name = std_cells.name(std_cell);
            auto blockages = iccad_2020.blockages(std_cell_name);
            for(auto blockage : blockages)
            {
                auto layer = library.find_layer_instance(blockage.layer_name);
                library.add_blockage(std_cell_name + ":" + blockage.blockage_name, std_cell, layer, blockage.demand);
            }
        }
    }
}
