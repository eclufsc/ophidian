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

#include "GlobalRouting.h"

namespace ophidian
{
namespace routing
{

GlobalRouting::GlobalRouting(const ophidian::circuit::Netlist &netlist):
    mNet2Region{netlist.makeAggregation<GlobalRouting::region_type>(ophidian::circuit::Net(), mRegions)}
{
}

GlobalRouting::net_region_view_type GlobalRouting::regions(const GlobalRouting::net_type &net) const
{
    return mNet2Region.parts(net);
}

GlobalRouting::net_type GlobalRouting::net(const GlobalRouting::region_type &region) const
{
    return mNet2Region.whole(region);
}

GlobalRouting::box_type GlobalRouting::box(const GlobalRouting::region_type &region) const
{
    return mBox[region];
}

GlobalRouting::layer_type GlobalRouting::layer(const GlobalRouting::region_type &region) const
{
    return mLayer[region];
}

GlobalRouting::region_container_type::const_iterator GlobalRouting::begin_region() const noexcept
{
    return mRegions.begin();
}

GlobalRouting::region_container_type::const_iterator GlobalRouting::end_region() const noexcept
{
    return mRegions.end();
}

GlobalRouting::region_container_type::size_type GlobalRouting::size_region() const noexcept
{
    return mRegions.size();
}

GlobalRouting::region_type GlobalRouting::add_region(const GlobalRouting::box_type &box, const GlobalRouting::layer_type &layer, const GlobalRouting::net_type &net)
{
    auto region = mRegions.add();
    mBox[region] = box;
    mLayer[region] = layer;
    mNet2Region.addAssociation(net, region);
    return region;
}

entity_system::EntitySystem<GlobalRouting::region_type>::NotifierType *GlobalRouting::notifier(GlobalRouting::region_type) const
{
    return mRegions.notifier();
}




} // namespace routing
} // namespace ophidian
