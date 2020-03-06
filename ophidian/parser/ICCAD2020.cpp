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

#include "AdjacentCutSpacing.h"
#include "CornerSpacing.h"
#include "EndOfLine.h"
#include "ICCAD2020.h"
#include "ParallelRunLength.h"
#include <ophidian/util/Units.h>
#include <fstream>

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
                m_grid_dimensions = std::make_tuple(std::stoi(tokens.at(3)),
                                                    std::stoi(tokens.at(4)),
                                                    1);
            else if(tokens.at(0) == "NumLayer")
            {
                auto num_layers = std::stoi(tokens.at(1));
                for(unsigned int i = 0; i < num_layers; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    auto direction = (tokens.at(3) == "H") ? Layer::Direction::HORIZONTAL : Layer::Direction::VERTICAL;
                    EndOfLine eol(micron_t{0.0},
                                  micron_t{0.0},
                                  micron_t{0.0});
                    ParallelEdge parallelEdge{micron_t{0.0},
                                              micron_t{0.0},
                                              micron_t{0.0},
                                              micron_t{0.0},
                                              micron_t{0.0}};
                    std::map<std::pair<micron_t, micron_t>, micron_t> my_map;
                    my_map.emplace(std::make_pair(micron_t{0.0}, micron_t{0.0}), micron_t{0.0});
                    ParallelRunLength parallelRunLength(micron_t{0.0},
                                                        micron_t{0.0},
                                                        my_map);
                    AdjacentCutSpacing adjacentCutSpacing(micron_t{0.0}, 1, micron_t{0.0});
                    CornerSpacing cornerSpacing(micron_t{0.0}, std::vector<std::pair<micron_t, micron_t>>{std::make_pair(micron_t{0.0}, micron_t{0.0})});
                    m_layers.push_back({tokens.at(1),
                                Layer::Type::ROUTING,
                                direction,
                                micron_t{0.0},
                                micron_t{0.0},
                                micron_t{0.0},
                                micron_t{0.0},
                                micron_t{0.0},
                                micron_t{0.0},
                                eol,
                                parallelEdge,
                                parallelRunLength,
                                adjacentCutSpacing,
                                cornerSpacing});
                    m_layer_capacity[tokens.at(1)] = std::stoi(tokens.at(4));
                }
            }
            else if(tokens.at(0) == "NumNonDefaultSupplyGGrid")
            {
                auto num_grids = std::stoi(tokens.at(1));
                for(unsigned int i = 0; i < num_grids; i++)
                {
                    std::getline(infile, line);
                    iss = std::istringstream(line);
                    tokens = {std::istream_iterator<std::string>{iss},
                              std::istream_iterator<std::string>{}};
                    auto gcellIndex = std::make_tuple(std::stoi(tokens.at(0)),
                                                      std::stoi(tokens.at(1)),
                                                      std::stoi(tokens.at(2)));
                    m_gcell_ndf_supply.push_back(std::make_pair(gcellIndex,
                                                                std::stoi(tokens.at(3))));
                }
            }
        }
    }
}
