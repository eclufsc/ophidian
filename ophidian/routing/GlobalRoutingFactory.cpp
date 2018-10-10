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
#include "ophidian/util/Units.h"
#include <unordered_map>

namespace ophidian
{
namespace routing
{
namespace factory
{
    void make_global_routing(ophidian::routing::GlobalRouting &globalRouting, const Library &library, const ophidian::circuit::Netlist &netlist, const ophidian::parser::Guide &guide) noexcept
    {
        for(auto net : guide.nets()){
            auto net_instance = netlist.find_net(net.name());
            for(auto region : net.regions()){
                auto layer_instance = library.find_layer_instance(region.metal());
                globalRouting.add_region(region.region(), layer_instance, net_instance);
            }
        }
    }
}
}
}
