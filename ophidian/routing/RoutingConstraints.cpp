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

#include "RoutingConstraints.h"

namespace ophidian::routing
{
    RoutingConstraints::RoutingConstraints(const ophidian::circuit::Netlist &netlist):
        m_min_net_layer{netlist.make_property_net<layer_type>()}
    {
    }

    RoutingConstraints::layer_type RoutingConstraints::min_net_layer(RoutingConstraints::net_type net)
    {
        return m_min_net_layer[net];
    }

    //order of parameter does not matter
    RoutingConstraints::demand_type RoutingConstraints::extra_demand_same_grid(RoutingConstraints::macro_name_type m1, RoutingConstraints::macro_name_type m2, RoutingConstraints::layer_name_type l){
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

    //order of parameter does not matter
    RoutingConstraints::demand_type RoutingConstraints::extra_demand_adj_grid(RoutingConstraints::macro_name_type m1, RoutingConstraints::macro_name_type m2, RoutingConstraints::layer_name_type l){
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

    unsigned int RoutingConstraints::max_cell_movement()
    {
        return m_max_cell_move;
    }

    void RoutingConstraints::resize_ndf_constraints(std::size_t size_x, std::size_t size_y, std::size_t size_z)
    {
        m_size_x = size_x;
        m_size_y = size_y;
        m_size_z = size_z;
        m_ndf_constraint.resize(m_size_x * m_size_y * m_size_z);
    }

    void RoutingConstraints::set_max_cell_movement(unsigned int max_move)
    {
        m_max_cell_move = max_move;
    }

    void RoutingConstraints::set_ndf_constraint(RoutingConstraints::index_type x, RoutingConstraints::index_type y, RoutingConstraints::index_type z, RoutingConstraints::demand_type val)
    {
        m_ndf_constraint.at(x + y * m_size_y + z * m_size_x * m_size_y) = val;
    }

    RoutingConstraints::demand_type RoutingConstraints::ndf_constraint(RoutingConstraints::index_type x, RoutingConstraints::index_type y, RoutingConstraints::index_type z) const
    {
        return m_ndf_constraint.at(x + y * m_size_y + z * m_size_x * m_size_y);
    }

    void RoutingConstraints::set_min_net_layer(RoutingConstraints::net_type net, RoutingConstraints::layer_type layer)
    {
        m_min_net_layer[net] = layer;
    }

    void RoutingConstraints::set_same_grid_extra_demand(RoutingConstraints::macro_name_type m1_name, RoutingConstraints::macro_name_type m2_name, RoutingConstraints::layer_name_type l_name, RoutingConstraints::demand_type d)
    {
        m_extra_demand_same_grid_map.insert({m1_name+":"+m2_name+":"+l_name, d});
    }

    void RoutingConstraints::set_adj_grid_extra_demand(RoutingConstraints::macro_name_type m1_name, RoutingConstraints::macro_name_type m2_name, RoutingConstraints::layer_name_type l_name, RoutingConstraints::demand_type d)
    {
        m_extra_demand_adj_grid_map.insert({m1_name+":"+m2_name+":"+l_name, d});
    }
}
