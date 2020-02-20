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

#include "GlobalRoutingFactory.h"
#include <unordered_map>

namespace ophidian::routing::factory
{
    void make_global_routing(ophidian::routing::GlobalRouting &globalRouting, const Library &library, const ophidian::circuit::Netlist &netlist, const ophidian::parser::Guide &guide) noexcept
    {
        for(auto net : guide.nets()){
            auto net_instance = netlist.find_net(net.name());
            for(auto region : net.regions()){
                auto layer_instance = library.find_layer_instance(region.metal());
                globalRouting.add_segment(region.region(), layer_instance, net_instance);
            }
        }
    }

    void make_global_routing(ophidian::routing::GlobalRouting& globalRouting, const ophidian::routing::Library & library, const ophidian::parser::Def& def) noexcept
    {
        //getting GCell information from DEF
        auto gcell_x_axis = def.gcell_x_axis();
        auto gcell_y_axis = def.gcell_y_axis();
        
        
        
        
        
        // library.set_gcell_coordinates(gcell_x_axis, gcell_y_axis);

        for (auto x_index = 0; x_index < gcell_x_axis.size() - 1; x_index++) {
            for (auto y_index = 0; y_index < gcell_y_axis.size() - 1; y_index++) {
                // auto min_corner = Library::point_type{gcell_x_axis.at(x_index), gcell_y_axis.at(y_index)}; 
                // auto max_corner = Library::point_type{gcell_x_axis.at(x_index + 1), gcell_y_axis.at(y_index + 1)};
                // library.add_gcell(min_corner, max_corner);
            }
        }

    }
}
