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
    GlobalRouting::GlobalRouting(const ophidian::circuit::Netlist &netlist):
        m_gr_segments{},
        m_gr_segment_box{m_gr_segments},
        m_gr_segment_layers{m_gr_segments},
        m_net_to_gr_segment{netlist.make_aggregation_net<GlobalRouting::gr_segment_type>(m_gr_segments)}
    {
    }

    GlobalRouting::net_segment_view_type GlobalRouting::segments(const GlobalRouting::net_type &net) const
    {
        return m_net_to_gr_segment.parts(net);
    }

    GlobalRouting::net_type GlobalRouting::net(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_net_to_gr_segment.whole(segment);
    }

    const GlobalRouting::segment_geometry_type& GlobalRouting::box(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_box[segment];
    }

    const GlobalRouting::layer_type GlobalRouting::layer(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_layers[segment];
    }

    void GlobalRouting::create_gcell_graph(GlobalRouting::unit_container_type x, GlobalRouting::unit_container_type y, GlobalRouting::index_type z)
    {
        m_gcell_graph.reset(new gcell_graph_type(x, y,z));
    }

    GlobalRouting::segment_container_type::const_iterator GlobalRouting::begin_segment() const noexcept
    {
        return m_gr_segments.begin();
    }

    GlobalRouting::segment_container_type::const_iterator GlobalRouting::end_segment() const noexcept
    {
        return m_gr_segments.end();
    }

    GlobalRouting::segment_container_type::size_type GlobalRouting::size_segment() const noexcept
    {
        return m_gr_segments.size();
    }

    GlobalRouting::gr_segment_type GlobalRouting::add_segment(const GlobalRouting::segment_geometry_type &geometry, const GlobalRouting::layer_type &layer, const GlobalRouting::net_type &net)
    {
        auto segment = m_gr_segments.add();
        m_gr_segment_box[segment] = geometry;
        m_gr_segment_layers[segment] = layer;
        m_net_to_gr_segment.addAssociation(net, segment);
        return segment;
    }

    entity_system::EntitySystem<GlobalRouting::gr_segment_type>::NotifierType *GlobalRouting::notifier(GlobalRouting::gr_segment_type) const
    {
        return m_gr_segments.notifier();
    }
}
