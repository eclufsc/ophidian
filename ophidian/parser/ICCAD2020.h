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

#ifndef OPHIDIAN_PARSER_ICCAD2020_H
#define OPHIDIAN_PARSER_ICCAD2020_H

#include "Component.h"
#include "Layer.h"
#include "Macro.h"
#include "Net.h"

namespace ophidian::parser
{
    /**
     * This is an encapsulation of an ICCAD2020 input 
     * described on a txt file to present max cell move, grid 
     * structure, supply and demand for each grid cell, layer 
     * informations, placement and routing constraints of a 
     * given circuit in ausable way.
     */
    class ICCAD2020
    {
    public:
        // class member types
        template <class T> using container_type     = std::vector<T>;
        template <class K, class V> using map_type  = std::map<K,V>;
        template <class K, class V> using pair_type = std::pair<K, V>;

        using gcell_index                           = std::tuple<int, int, int>;
        using layer_container_type                  = container_type<Layer>;
        using macro_container_type                  = container_type<Macro>;
        using net_container_type                    = container_type<Net>;
        using component_container_type              = container_type<Component>;
        using layer_name_type                       = std::string;
        using layer_capacity_map                    = map_type<layer_name_type, int>;
        using gcell_ndf_supply                      = container_type< pair_type<gcell_index, int> >;

        ICCAD2020() = default;

        ICCAD2020(const ICCAD2020&) = default;
        ICCAD2020& operator=(const ICCAD2020&) = default;

        ICCAD2020(ICCAD2020&&) = default;
        ICCAD2020& operator=(ICCAD2020&&) = default;

        ICCAD2020(const std::string& iccad2020_file);

    private:
        void read_file(const std::string& iccad2020_file);

        unsigned int m_max_cell_move;
        gcell_index m_grid_dimensions;
        layer_capacity_map m_layer_capacity;
        gcell_ndf_supply m_gcell_ndf_supply;

        layer_container_type m_layers{};
        component_container_type m_components{};
        macro_container_type m_macros{};
        net_container_type       m_nets{};
        //vector of positions
        //routes
    };
}

#endif
