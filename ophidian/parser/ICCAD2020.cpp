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

#include "ICCAD2020.h"
#include "Macro.h"
#include <ophidian/util/Units.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

using micron_t = ophidian::util::micrometer_t;

namespace ophidian::parser
{
    ICCAD2020::ICCAD2020(const std::string& iccad2020_file):
        m_layers{},
        m_components{},
        m_macros{},
        m_nets{}
    {
        read_file(iccad2020_file);
    }

    const unsigned int ICCAD2020::max_cell_move() const noexcept {
        return m_max_cell_move;
    }

    const ICCAD2020::gcell_index & ICCAD2020::grid_dimensions() const noexcept {
        return m_grid_dimensions;
    }

    const std::pair<int, int> & ICCAD2020::grid_origin() const noexcept {
        return m_grid_origin;
    }

    const std::pair<int, int> & ICCAD2020::grid_boundary() const noexcept {
        return m_grid_boundary;
    }

    const ICCAD2020::layer_container_type & ICCAD2020::layers() const noexcept {
        return m_layers;
    }

    const ICCAD2020::component_container_type & ICCAD2020::components() const noexcept {
        return m_components;
    }

    const ICCAD2020::net_container_type & ICCAD2020::nets() const noexcept {
        return m_nets;
    }

    const ICCAD2020::macro_container_type & ICCAD2020::macros() const noexcept {
        return m_macros;
    }

    const ICCAD2020::gcell_ndf_supply & ICCAD2020::gcell_non_default_supply() const noexcept {
        return m_gcell_ndf_supply;
    }

    const ICCAD2020::segment_container_type & ICCAD2020::segments() const noexcept {
        return m_segments;
    }

    ICCAD2020::demand_type ICCAD2020::extra_demand_same_grid(macro_name_type m1, macro_name_type m2, layer_name_type l){
        auto key1 = m1+":"+m2+":"+l;
        auto search = m_extra_demand_same_grid_map.find(key1);
        if(search != m_extra_demand_same_grid_map.end())
            return search->second;
        else{
            auto key2 = m2+":"+m1+":"+l;
            search = m_extra_demand_same_grid_map.find(key2);
            if(search != m_extra_demand_same_grid_map.end())
                return search->second;
            else
                return 0;
        }
    }

    ICCAD2020::demand_type ICCAD2020::extra_demand_adj_grid(macro_name_type m1, macro_name_type m2, layer_name_type l){
        auto key1 = m1+":"+m2+":"+l;
        auto search = m_extra_demand_adj_grid_map.find(key1);
        if(search != m_extra_demand_adj_grid_map.end())
            return search->second;
        else{
            auto key2 = m2+":"+m1+":"+l;
            search = m_extra_demand_adj_grid_map.find(key2);
            if(search != m_extra_demand_adj_grid_map.end())
                return search->second;
            else
                return 0;
        }
    }

    void ICCAD2020::read_file(const std::string& iccad2020_file)
    {
        std::ifstream infile(iccad2020_file);
        std::string line;
        std::istringstream iss;
        while(std::getline(infile, line))
        {
            iss = std::istringstream(line);
            std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                            std::istream_iterator<std::string>{}};
            if(tokens.at(0) == "MaxCellMove")
                m_max_cell_move = std::stoi(tokens.at(1));
            else if(tokens.at(0) == "GGridBoundaryIdx")
            {
                m_grid_dimensions = std::make_tuple(std::stoi(tokens.at(4)),
                                                    std::stoi(tokens.at(3)),
                                                    1);
                m_grid_origin = std::make_pair(std::stoi(tokens.at(2)), std::stoi(tokens.at(1)));
                m_grid_boundary = std::make_pair(std::stoi(tokens.at(4)), std::stoi(tokens.at(3)));
            }
            else if(tokens.at(0) == "NumLayer")
            {
                auto num_layers = std::stoi(tokens.at(1));
                m_layers.reserve(num_layers);
                m_grid_dimensions = std::make_tuple(std::get<0>(m_grid_dimensions),
                                                    std::get<1>(m_grid_dimensions),
                                                    num_layers);
                for(unsigned int i = 0; i < num_layers; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    auto direction = (tokens.at(3) == "H") ? Layer::Direction::HORIZONTAL : Layer::Direction::VERTICAL;
                    m_layers.push_back({tokens.at(1),
                                Layer::Type::ROUTING,
                                direction,
                                std::stoi(tokens.at(2)),
                                std::stoi(tokens.at(4))});
                }
            }
            else if(tokens.at(0) == "NumNonDefaultSupplyGGrid")
            {
                auto num_grids = std::stoi(tokens.at(1));
                m_gcell_ndf_supply.reserve(num_grids);
                for(unsigned int i = 0; i < num_grids; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    auto gcellIndex = std::make_tuple(std::stoi(tokens.at(1)),
                                                      std::stoi(tokens.at(0)),
                                                      std::stoi(tokens.at(2)));
                    m_gcell_ndf_supply.push_back(std::make_pair(gcellIndex, std::stoi(tokens.at(3))));
                }
            }
            else if(tokens.at(0) == "NumMasterCell")
            {
                auto num_macros = std::stoi(tokens.at(1));
                m_macros.reserve(num_macros);
                for(unsigned int i = 0; i < num_macros; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    std::string macro_name = tokens.at(1);
                    auto macro_origin = macro_type::micrometer_point_type{micron_t{0}, micron_t{0}};
                    m_macros.emplace_back(macro_name, "", macro_type::foreign_type{}, macro_origin, macro_origin, "", macro_type::pin_container_type{}, macro_type::obstruction_map_type{});
                    auto pin_count = std::stoi(tokens.at(2));
                    auto blockage_count = std::stoi(tokens.at(3));
                    for(unsigned int j = 0; j < pin_count; j++)
                    {
                        std::getline(infile, line);
                        iss = std::istringstream(line);
                        tokens = {std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
                        Macro::Pin::port_map_type map;
                        auto pin_name = tokens.at(1);
                        m_macros.back().pins().emplace_back(pin_name, pin_type::Direction::NA, pin_type::port_map_type{}, tokens.at(2));
                    }
                    for(unsigned int k = 0; k < blockage_count; k++)
                    {
                        std::getline(infile, line);
                        iss = std::istringstream(line);
                        tokens = {std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
                        ICCADBlockage blockage(tokens.at(1), macro_name, tokens.at(2), std::stoi(tokens.at(3)));
                        m_iccad_blockage_map[macro_name].push_back(blockage);
                    }
                }
            }
            else if(tokens.at(0) == "NumNeighborCellExtraDemand")
            {
                auto extra_count = std::stoi(tokens.at(1));
                for (unsigned int i = 0; i < extra_count; i++) {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    if(tokens.at(0) == "sameGGrid")
                        m_extra_demand_same_grid_map.insert({tokens.at(1)+":"+tokens.at(2)+":"+tokens.at(3), std::stoi(tokens.at(4))});
                    else if(tokens.at(0) == "adjHGGrid")
                        m_extra_demand_adj_grid_map.insert({tokens.at(1)+":"+tokens.at(2)+":"+tokens.at(3), std::stoi(tokens.at(4))});
                }
            }
            else if(tokens.at(0) == "NumCellInst")
            {
                auto num_cells = std::stoi(tokens.at(1));
                m_components.reserve(num_cells);
                    for(unsigned int i = 0; i < num_cells; i++)
                    {
                        std::getline(infile, line);
                        iss = std::istringstream(line);
                        tokens = {std::istream_iterator<std::string>{iss},
                                  std::istream_iterator<std::string>{}};
                        auto cell_name = tokens.at(1);
                        auto macro_name = tokens.at(2);
                        auto x = std::stod(tokens.at(4));
                        auto y = std::stod(tokens.at(3));
                        auto position = component_type::database_unit_point_type{component_type::database_unit_type{x}, component_type::database_unit_type{y}};
                        auto fixed = tokens.at(5) != "Movable";
                        m_components.push_back(component_type{cell_name, macro_name, Component::Orientation::N, position, fixed});
                    }
            }
            else if(tokens.at(0) == "NumNets")
            {
                auto num_nets = std::stoi(tokens.at(1));
                m_nets.reserve(num_nets);
                for(unsigned int i = 0; i < num_nets; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    auto net_name = tokens.at(1);
                    auto min_layer = tokens.at(3);
                    auto num_pins = std::stoi(tokens.at(2));
                    auto net_pins = net_type::pin_container_type{};
                    net_pins.reserve(num_pins);
                    for(unsigned int pin_index = 0; pin_index < num_pins; pin_index++)
                    {
                        std::getline(infile, line);
                        iss = std::istringstream(line);tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                        auto component_pin_name = tokens.at(1);
                        std::vector<std::string> split_strings;
                        boost::split(split_strings, component_pin_name, boost::is_any_of("/"));
                        net_pins.push_back({split_strings.at(0), split_strings.at(1)});
                    }
                    m_nets.push_back(net_type{net_name, net_pins, min_layer});
                }
            }
            else if(tokens.at(0) == "NumRoutes")
            {
                auto num_segments = std::stoi(tokens.at(1));
                for(unsigned int i = 0; i < num_segments; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    m_segments.push_back({std::stoi(tokens.at(1)), std::stoi(tokens.at(0)), std::stoi(tokens.at(2)),
                                          std::stoi(tokens.at(4)), std::stoi(tokens.at(3)), std::stoi(tokens.at(5)), tokens.at(6)});
                }
            }
        }
    }
}
