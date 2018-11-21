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

namespace ophidian::routing
{
    GlobalRouting::GlobalRouting(const ophidian::circuit::Netlist &netlist) noexcept:
        m_regions{},
        m_region_geometries{m_regions},
        m_region_layers{m_regions},
        m_net_to_regions{netlist.make_aggregation_net<GlobalRouting::region_type>(m_regions)}
    {
    }

    GlobalRouting::net_region_view_type GlobalRouting::regions(const GlobalRouting::net_type &net) const
    {
        return m_net_to_regions.parts(net);
    }

    GlobalRouting::net_type GlobalRouting::net(const GlobalRouting::region_type &region) const
    {
        return m_net_to_regions.whole(region);
    }

    GlobalRouting::region_geometry_type& GlobalRouting::geometry(const GlobalRouting::region_type &region)
    {
        return m_region_geometries[region];
    }

    const GlobalRouting::region_geometry_type& GlobalRouting::geometry(const GlobalRouting::region_type &region) const
    {
        return m_region_geometries[region];
    }

    GlobalRouting::layer_type& GlobalRouting::layer(const GlobalRouting::region_type &region)
    {
        return m_region_layers[region];
    }

    const GlobalRouting::layer_type& GlobalRouting::layer(const GlobalRouting::region_type &region) const
    {
        return m_region_layers[region];
    }

    GlobalRouting::region_container_type::const_iterator GlobalRouting::begin_region() const noexcept
    {
        return m_regions.begin();
    }

    GlobalRouting::region_container_type::const_iterator GlobalRouting::end_region() const noexcept
    {
        return m_regions.end();
    }

    GlobalRouting::region_container_type::size_type GlobalRouting::size_region() const noexcept
    {
        return m_regions.size();
    }

    GlobalRouting::region_type GlobalRouting::add_region(const GlobalRouting::region_geometry_type &geometry, const GlobalRouting::layer_type &layer, const GlobalRouting::net_type &net)
    {
        auto region = m_regions.add();
        m_region_geometries[region] = geometry;
        m_region_layers[region] = layer;
        m_net_to_regions.addAssociation(net, region);
        return region;
    }

    entity_system::EntitySystem<GlobalRouting::region_type>::NotifierType *GlobalRouting::notifier_region() const noexcept
    {
        return m_regions.notifier();
    }
}
