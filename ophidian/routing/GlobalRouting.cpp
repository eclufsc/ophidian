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
    GlobalRouting::GlobalRouting(const ophidian::circuit::Netlist &netlist, const ophidian::routing::Library &library):
        m_gr_segments{},
        m_gr_segment_box{m_gr_segments},
        m_gr_segment_layers_start{m_gr_segments},
        m_gr_segment_layers_end{m_gr_segments},
        m_gr_segment_gcell_start{m_gr_segments},
        m_gr_segment_gcell_end{m_gr_segments},
        m_net_to_gr_segment{netlist.make_aggregation_net<GlobalRouting::gr_segment_type>(m_gr_segments)},
        m_library{library}
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

    const GlobalRouting::layer_type GlobalRouting::layer_start(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_layers_start[segment];
    }

    const GlobalRouting::layer_type GlobalRouting::layer_end(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_layers_end[segment];
    }

    const GlobalRouting::gcell_type GlobalRouting::gcell_start(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_gcell_start[segment];
    }

    const GlobalRouting::gcell_type GlobalRouting::gcell_end(const GlobalRouting::gr_segment_type &segment) const
    {
        return m_gr_segment_gcell_end[segment];
    }

    void GlobalRouting::create_gcell_graph(GlobalRouting::unit_container_type x, GlobalRouting::unit_container_type y, GlobalRouting::index_type z, GCellGraph::scalar_container_type capacities)
    {
        m_gcell_graph.reset(new gcell_graph_type(x, y, z, capacities));
    }

    void GlobalRouting::create_gcell_graph(const ophidian::routing::Library & library, GlobalRouting::unit_container_type x, GlobalRouting::unit_container_type y, GlobalRouting::index_type z)
    {
        m_gcell_graph.reset(new gcell_graph_type(library, x, y, z));
    }

    GlobalRouting::gcell_graph_ptr_type GlobalRouting::gcell_graph()
    {
        return m_gcell_graph;
    }

    GlobalRouting::gcell_container_type GlobalRouting::gcells(const GlobalRouting::net_type& net) const{
        auto segments = m_net_to_gr_segment.parts(net);
        std::vector<ophidian::routing::GCell> gcells;
        for(auto segment : segments)
        {
            auto layer_start = m_gr_segment_layers_start[segment];
            auto layer_end = m_gr_segment_layers_end[segment];
            auto min_index = m_library.layerIndex(layer_start);
            auto max_index = m_library.layerIndex(layer_end);
            for(auto i = min_index; i <= max_index; i++)
            {
                auto box = m_gr_segment_box[segment];
                m_gcell_graph->intersect(gcells, box, i-1);
            }
        }
        //remove duplicated gcells
        std::sort(gcells.begin(), gcells.end(), [&](auto &lhs, auto &rhs){return m_gcell_graph->id(lhs) < m_gcell_graph->id(rhs);});
        gcells.erase(std::unique(gcells.begin(), gcells.end()), gcells.end());
        return gcells;
    }

    const GlobalRouting::scalar_type GlobalRouting::wirelength_in_gcell(const GlobalRouting::net_type & net) const{
        return gcells(net).size();
    }

    const GlobalRouting::scalar_type GlobalRouting::wirelength_in_gcell(const GlobalRouting::net_container_type & nets) const{
        scalar_type wirelength = 0;
        for(auto net : nets)
        {
            wirelength += wirelength_in_gcell(net);
        }
        return wirelength;
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

    GlobalRouting::gr_segment_type GlobalRouting::add_segment(const GlobalRouting::segment_geometry_type &geometry, const GlobalRouting::layer_type &layer_start, const GlobalRouting::layer_type &layer_end, const GlobalRouting::net_type &net)
    {
        auto segment = m_gr_segments.add();
        m_gr_segment_box[segment] = geometry;
        m_net_to_gr_segment.addAssociation(net, segment);

        auto s_index = m_library.layerIndex(layer_start);
        auto e_index = m_library.layerIndex(layer_end);
        //make sure layer_start is always below layer_end, it helps to set their gcells.
        m_gr_segment_layers_start[segment] = (s_index <= e_index) ? layer_start : layer_end;
        m_gr_segment_layers_end[segment] = (s_index <= e_index) ? layer_end : layer_start;

        if(m_gcell_graph)
            set_gcells(segment);
        return segment;
    }

    void GlobalRouting::increase_demand(const GlobalRouting::net_type& net)
    {
        auto gcell_container = gcells(net);
        for(auto gcell : gcell_container)
            m_gcell_graph->change_net_demand(gcell, 1);
    }

    void GlobalRouting::decrease_demand(const GlobalRouting::net_type& net)
    {
        auto gcell_container = gcells(net);
        for(auto gcell : gcell_container)
            m_gcell_graph->change_net_demand(gcell, -1);
    }

    entity_system::EntitySystem<GlobalRouting::gr_segment_type>::NotifierType *GlobalRouting::notifier(GlobalRouting::gr_segment_type) const
    {
        return m_gr_segments.notifier();
    }

    void GlobalRouting::set_gcells(const GlobalRouting::gr_segment_type& segment)
    {
        gcell_container_type gcells;
        gcell_type start_gcell, end_gcell;
        auto box_segment = m_gr_segment_box[segment];
        auto start_index = m_library.layerIndex(m_gr_segment_layers_start[segment]);
        auto end_index = m_library.layerIndex(m_gr_segment_layers_end[segment]);

        //same layer
        if(start_index == end_index)
        {
            m_gcell_graph->intersect(gcells, box_segment, start_index-1);
            std::pair<int, int> min{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
            std::pair<int, int> max{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
            for(auto gcell : gcells)
            {
                auto graph_node = m_gcell_graph->graph_node(gcell);
                auto node_position = m_gcell_graph->position(graph_node);
                min.first = std::min(node_position.get<0>(), min.first);
                min.second = std::min(node_position.get<1>(), min.second);
                max.first = std::max(node_position.get<0>(), max.first);
                max.second = std::max(node_position.get<1>(), max.second);
            }
            start_gcell = m_gcell_graph->gcell(min.first, min.second, start_index-1);
            end_gcell = m_gcell_graph->gcell(max.first, max.second, end_index-1);
        } else {
            //via
            m_gcell_graph->intersect(gcells, box_segment, start_index-1);
            start_gcell = gcells.back();
            
            gcells.clear();
            m_gcell_graph->intersect(gcells, box_segment, end_index-1);
            end_gcell = gcells.back();
        }
        m_gr_segment_gcell_start[segment] = start_gcell;
        m_gr_segment_gcell_end[segment] = end_gcell;
        //update_gcells_demand(segment, 1);
    }

    void GlobalRouting::unroute(const net_type& net){
        std::vector<gr_segment_type> segments_to_remove;
        for(auto segment : segments(net))
        {
            segments_to_remove.push_back(segment);
            update_gcells_demand(segment, -1);
        }

        for(auto segment : segments_to_remove){
            m_gr_segments.erase(segment);
        }
    }

    void GlobalRouting::update_gcells_demand(const gr_segment_type & segment, const int delta){
        auto layer_start = m_gr_segment_layers_start[segment];
        auto layer_end = m_gr_segment_layers_end[segment];
        auto gcell_start = m_gr_segment_gcell_start[segment];
        auto gcell_end = m_gr_segment_gcell_end[segment];
        auto box_segment = m_gr_segment_box[segment];
        auto start_index = m_library.layerIndex(m_gr_segment_layers_start[segment]);
        auto end_index = m_library.layerIndex(m_gr_segment_layers_end[segment]);
        
        gcell_container_type gcells;
        if(layer_start == layer_end){
            m_gcell_graph->intersect(gcells, box_segment, start_index-1);
            std::pair<int, int> min{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
            std::pair<int, int> max{std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
            for(auto gcell : gcells)
            {
                m_gcell_graph->change_net_demand(gcell, delta);
            }
        }else{
            // via
            auto start_node = m_gcell_graph->graph_node(gcell_start);
            auto end_node = m_gcell_graph->graph_node(gcell_end);
            auto current_node = m_gcell_graph->up_node(start_node);
            while (current_node != end_node)
            {
                auto gcell = m_gcell_graph->gcell(current_node);
                m_gcell_graph->change_net_demand(gcell, delta);
                current_node = m_gcell_graph->up_node(current_node);
            }
        }
    }

    bool GlobalRouting::is_connected(const net_type & net, const gcell_container_type & pin_gcells) {
        auto net_graph = graph_type{};
        std::unordered_map<gcell_type, node_type, entity_system::EntityBaseHash> gcell_to_node;
        for (auto gcell : pin_gcells) {
            if (gcell_to_node.find(gcell) == gcell_to_node.end()) {
                auto node = net_graph.addNode();
                gcell_to_node[gcell] = node;

                auto point = m_gcell_graph->center_of_box(gcell);
                auto layer = m_gcell_graph->layer_index(gcell);
            }
        }
        for (auto segment : m_net_to_gr_segment.parts(net)) {
            auto gcell_start = m_gr_segment_gcell_start[segment]; 
            auto gcell_end = m_gr_segment_gcell_end[segment];
            auto start_node = node_type{};
            auto end_node = node_type{};
            if (gcell_to_node.find(gcell_start) == gcell_to_node.end()) {
                start_node = net_graph.addNode();
                gcell_to_node[gcell_start] = start_node;
                
            } else {
                start_node = gcell_to_node[gcell_start];
            }
                auto start_point = m_gcell_graph->center_of_box(gcell_start);
                auto start_layer = m_gcell_graph->layer_index(gcell_start);
            if (gcell_to_node.find(gcell_end) == gcell_to_node.end()) {
                end_node = net_graph.addNode();
                gcell_to_node[gcell_end] = end_node;
                
            } else {
                end_node = gcell_to_node[gcell_end];
            }
                auto point = m_gcell_graph->center_of_box(gcell_end);
                auto layer = m_gcell_graph->layer_index(gcell_start);
            net_graph.addEdge(start_node, end_node);
        }

        return lemon::connected(net_graph);
    }

}
