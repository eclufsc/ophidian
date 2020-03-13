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
    void make_global_routing(ophidian::routing::GlobalRouting &globalRouting, const Library &library, const ophidian::circuit::Netlist &netlist, const ophidian::parser::Guide &guide, const ophidian::parser::Def& def) noexcept
    {
        for(auto net : guide.nets()){
            auto net_instance = netlist.find_net(net.name());
            for(auto region : net.regions()){
                auto layer_instance = library.find_layer_instance(region.metal());
                globalRouting.add_segment(region.region(), layer_instance, net_instance);
            }
        }

        if( def.gcell_x_axis().size() != 0 && def.gcell_y_axis().size() != 0)
        {
            int z = library.index(library.highest_layer());
            globalRouting.create_gcell_graph(library, def.gcell_x_axis(), def.gcell_y_axis(), z);
        }else{
            // Design does not have GCELL definitions !!
            std::cout << "WARNING : This Design does not have GCELL definitions in .def file!" << std::endl;
            std::cout << "        : Global Routing does not have GCELL Graph" << std::endl;
        }
    }
}
