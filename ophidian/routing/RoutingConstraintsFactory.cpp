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

#include "RoutingConstraintsFactory.h"

namespace ophidian::routing::factory
{
    void make_routing_constraints(ophidian::routing::RoutingConstraints & routingConstraints,
                                  const ophidian::routing::Library & library,
                                  const ophidian::circuit::Netlist & netlist,
                                  const ophidian::parser::ICCAD2020 & iccad_2020) noexcept
    {
        routingConstraints.set_max_cell_movement(iccad_2020.max_cell_move());
        auto dimensions = iccad_2020.grid_dimensions();
        routingConstraints.resize_ndf_constraints(std::get<0>(dimensions), std::get<1>(dimensions), std::get<2>(dimensions));

        for(auto constraint : iccad_2020.same_grid_extra_demands())
            routingConstraints.set_same_grid_extra_demand(constraint.macro1_name, constraint.macro2_name, constraint.layer_name, constraint.extra_demand);

        for(auto constraint : iccad_2020.adj_grid_extra_demands())
            routingConstraints.set_adj_grid_extra_demand(constraint.macro1_name, constraint.macro2_name, constraint.layer_name, constraint.extra_demand);

        //get first layer
        int first_layer_index = std::numeric_limits<int>::max();
        std::string first_layer_name;
        for(auto layer : iccad_2020.layers())
            if(layer.index() < first_layer_index){
                first_layer_name = layer.name();
                first_layer_index = layer.index();
            }
        auto first_layer = library.find_layer_instance(first_layer_name);

        //set min layer constraints
        for(auto iccad_net : iccad_2020.nets())
        {
            auto net_name = iccad_net.name();
            auto net = netlist.find_net(net_name);
            auto min_layer_name = iccad_net.min_layer();
            if (min_layer_name == "NoCstr")
                routingConstraints.set_min_net_layer(net, first_layer);
            else
                routingConstraints.set_min_net_layer(net, library.find_layer_instance(min_layer_name));
        }

        for(auto ndf : iccad_2020.gcell_non_default_supply())
        {
            auto gcell_index_tuple = ndf.first;
            auto demand = ndf.second;
            //-1 to convert do GCell Graph index
            routingConstraints.set_ndf_constraint(std::get<0>(gcell_index_tuple)-1, std::get<1>(gcell_index_tuple)-1, std::get<2>(gcell_index_tuple)-1, ndf.second);
        }
    }
}
