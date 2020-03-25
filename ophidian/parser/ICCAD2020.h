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

    //Aditional informations about macros
    struct ICCADBlockage{
        std::string blockage_name;
        std::string macro_name;
        std::string layer_name;
        int demand;

        ICCADBlockage(std::string blk_name, std::string m_name, std::string l_name, int d){
            blockage_name = blk_name;
            macro_name = m_name;
            layer_name = l_name;
            demand = d;
        };
    };

    struct ExtraDemandConstraint{
        std::string macro1_name;
        std::string macro2_name;
        std::string layer_name;
        int extra_demand;

        ExtraDemandConstraint(std::string m1_name, std::string m2_name, std::string l_name, int demand){
            macro1_name = m1_name;
            macro2_name = m2_name;
            layer_name = l_name;
            extra_demand = demand;
        };
    };

    //Global routing provided information
    struct ICCADSegment{
        std::string net_name;
        std::tuple<int, int, int> start, end;
        ICCADSegment(int sx, int sy, int sz, int ex, int ey, int ez, std::string n_name){
            start = std::make_tuple(sx, sy, sz);
            end = std::make_tuple(ex, ey, ez);
            net_name = n_name;
        }
    };

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
        template <class K, class V> using map_type  = std::unordered_map<K,V>;
        template <class K, class V> using pair_type = std::pair<K, V>;

        using gcell_index                           = std::tuple<int, int, int>;
        using demand_type                           = int;
        using layer_container_type                  = container_type<Layer>;
        using macro_container_type                  = container_type<Macro>;
        using net_type                              = Net;
        using net_container_type                    = container_type<net_type>;
        using component_type                        = Component;
        using macro_type                            = Macro;
        using pin_type                              = Macro::Pin;
        using component_container_type              = container_type<component_type>;
        using blockage_type                         = ICCADBlockage;
        using blockage_container_type               = container_type<blockage_type>;
        using segment_type                          = ICCADSegment;
        using segment_container_type                = container_type<segment_type>;
        using layer_name_type                       = std::string;
        using macro_name_type                       = std::string;
        using same_grid_key_type                    = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using adj_grid_key_type                     = std::string;// cell1_name + ":" + cell2_name + ":" + layer_name;
        using extra_demands_type                    = container_type<ExtraDemandConstraint>;
        using blockage_map                          = map_type<macro_name_type, blockage_container_type>;
        using same_grid_map                         = map_type<same_grid_key_type, demand_type>;
        using adj_grid_map                          = map_type<adj_grid_key_type, demand_type>;
        using gcell_ndf_supply                      = container_type< pair_type<gcell_index, int> >;

        ICCAD2020() = default;

        ICCAD2020(const ICCAD2020&) = default;
        ICCAD2020& operator=(const ICCAD2020&) = default;

        ICCAD2020(ICCAD2020&&) = default;
        ICCAD2020& operator=(ICCAD2020&&) = default;

        ICCAD2020(const std::string& iccad2020_file);

        const unsigned int max_cell_move() const noexcept;

        const gcell_index & grid_dimensions() const noexcept;

        const std::pair<int, int> & grid_origin() const noexcept;

        const std::pair<int, int> & grid_boundary() const noexcept;

        const layer_container_type & layers() const noexcept;

        const component_container_type & components() const noexcept;

        const net_container_type & nets() const noexcept;

        const macro_container_type & macros() const noexcept;

        const gcell_ndf_supply & gcell_non_default_supply() const noexcept;

        const segment_container_type & segments() const noexcept;

        const extra_demands_type & same_grid_extra_demands() const noexcept;

        const extra_demands_type & adj_grid_extra_demands() const noexcept;

        const blockage_container_type blockages(std::string macro_name) const noexcept;
    private:
        void read_file(const std::string& iccad2020_file);

        unsigned int m_max_cell_move;
        gcell_index m_grid_dimensions;
        std::pair<int, int> m_grid_origin;
        std::pair<int, int> m_grid_boundary;
        gcell_ndf_supply m_gcell_ndf_supply;
        blockage_map m_iccad_blockage_map;

        extra_demands_type m_same_grid{};
        extra_demands_type m_adj_grid{};
        layer_container_type m_layers{};
        component_container_type m_components{};
        macro_container_type m_macros{};
        net_container_type m_nets{};
        segment_container_type m_segments{};
    };
}

#endif
