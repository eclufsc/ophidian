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
        m_library{library},
        m_net_lower_bound_wirelength{netlist.make_property_net<GlobalRouting::scalar_type>(0)}
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
        //set_gcell_cell_instances()
    }

    void GlobalRouting::create_gcell_graph(const ophidian::routing::Library & library, GlobalRouting::unit_container_type x, GlobalRouting::unit_container_type y, GlobalRouting::index_type z)
    {
        m_gcell_graph.reset(new gcell_graph_type(library, x, y, z));
        //set_gcell_cell_instances()
    }

    GlobalRouting::gcell_graph_ptr_type GlobalRouting::gcell_graph() const
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
            auto layer_start_index = m_library.layerIndex(layer_start);
            auto layer_end_index = m_library.layerIndex(layer_end);
            auto min_index = std::min(layer_start_index, layer_end_index);
            auto max_index = std::max(layer_start_index, layer_end_index);
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

    const GlobalRouting::scalar_type GlobalRouting::wirelength(const GlobalRouting::net_type & net) const{
        scalar_type wirelength = gcells(net).size();
        // if (wirelength == 0)
        //     return 1;
        // else    
            return wirelength;
    }

    const GlobalRouting::scalar_type GlobalRouting::wirelength(const GlobalRouting::net_container_type & nets) const{
        scalar_type wirelength = 0;
        for(auto net : nets)
        {
            wirelength += this->wirelength(net);
        }
        return wirelength;
    }

    const GlobalRouting::scalar_type GlobalRouting::number_of_vias(const net_type & net) const{
        scalar_type vias = 0;
        auto segments = m_net_to_gr_segment.parts(net);
        for(auto segment : segments)
        {
            auto layer_start = m_gr_segment_layers_start[segment];
            auto layer_end = m_gr_segment_layers_end[segment];
            auto layer_start_index = m_library.layerIndex(layer_start);
            auto layer_end_index = m_library.layerIndex(layer_end);
            if(layer_start != layer_end)
            {
                vias += (layer_end_index - layer_start_index);
            }
        }
        return vias;
    }

    const GlobalRouting::scalar_type GlobalRouting::number_of_vias(const net_container_type & nets) const{
        scalar_type vias = 0;
        for(auto net : nets)
        {
            vias += this->number_of_vias(net);
        }
        return vias;
    }

    const GlobalRouting::scalar_type GlobalRouting::lower_bound_wirelength(const net_type & net) const{
        return m_net_lower_bound_wirelength[net];
    }

    void GlobalRouting::lower_bound_wirelength(const net_type & net, scalar_type wl){
        m_net_lower_bound_wirelength[net] = wl;
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
        // if (gcell_container.empty() == true)
            /* find the gcell where this cell is placed and icrease its wirelength*/
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

                auto gcell_box = m_gcell_graph->box(gcell);
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

    void GlobalRouting::unroute(const net_type& net) {
        
        this->decrease_demand(net);

        std::vector<gr_segment_type> segments_to_remove;
        for(auto segment : this->segments(net))
            segments_to_remove.push_back(segment);
        
        for(auto segment : segments_to_remove)
            m_gr_segments.erase(segment);
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

    void GlobalRouting::set_gcell_cell_instances(ophidian::circuit::Netlist & netlist, ophidian::placement::Placement & placement)
    {
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++)
        {
            auto cell = *cell_it;
            auto location = placement.location(cell);
            auto gcell = m_gcell_graph->nearest_gcell(location, 0);
            m_gcell_graph->insert_cell_instance(gcell, cell);
        }
    }

    bool GlobalRouting::is_overflow() const
    {
        auto& graph = *this->m_gcell_graph;
        auto is_overflow = false;
        for (auto gcellIt = graph.begin_gcell(); gcellIt != graph.end_gcell(); gcellIt++)
        {
            // std::cout << "Cell: x: " << graph.position(graph.graph_node(*gcellIt)).get<0>() 
            // << " y: " << graph.position(graph.graph_node(*gcellIt)).get<1>() 
            // << " z: " << graph.position(graph.graph_node(*gcellIt)).get<2>() 
            // << " demand: " << graph.demand(*gcellIt) << " capacity: " << graph.capacity(*gcellIt) << std::endl;
            
            if (graph.is_overflow(*gcellIt))
            {
                is_overflow = true;
            }
        }
        return is_overflow;
    }

    bool GlobalRouting::is_overflow(const net_container_type& nets, net_container_type& ovfl_nets) const
    {
        auto& graph = *this->m_gcell_graph;
        auto is_overflow = false;

        for (const auto& net : nets)
        {
            auto net_gcells = this->gcells(net);
            for (const auto& gcell : net_gcells)
            {
                if (graph.is_overflow(gcell))
                {
                    ovfl_nets.push_back(net);
                    is_overflow = true;
                }
            }
        }
        return is_overflow;
    }

    bool GlobalRouting::is_connected(const net_type & net, const gcell_container_type & pin_gcells, std::string net_name) const
    {
        auto net_graph = graph_type{};
        std::unordered_map<gcell_type, node_type, entity_system::EntityBaseHash> gcell_to_node;
        /*if (net_name == "net1192") {
        std::cout << "pin gcells " << pin_gcells.size() << std::endl;
        std::cout << "segments " << m_net_to_gr_segment.parts(net).size() << std::endl;
        }*/
        for (auto gcell : pin_gcells) {
            if (gcell_to_node.find(gcell) == gcell_to_node.end()) {
                auto node = net_graph.addNode();
                gcell_to_node[gcell] = node;

                auto point = m_gcell_graph->center_of_box(gcell);
                auto layer = m_gcell_graph->layer_index(gcell);

                /*if (net_name == "net1192") {
                std::cout << "pin " << point.x().value() << "," << point.y().value() << "," << layer << std::endl;
                }*/
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
            if (gcell_to_node.find(gcell_end) == gcell_to_node.end()) {
                end_node = net_graph.addNode();
                gcell_to_node[gcell_end] = end_node;
            } else {
                end_node = gcell_to_node[gcell_end];
            }
            net_graph.addEdge(start_node, end_node);

            auto start_point = m_gcell_graph->center_of_box(gcell_start);
            auto start_layer = m_gcell_graph->layer_index(gcell_start);
            auto end_point = m_gcell_graph->center_of_box(gcell_end);
            auto end_layer = m_gcell_graph->layer_index(gcell_end);
            /*if (net_name == "net1192") {
             std::cout << "segment " << start_point.x().value() << "," << start_point.y().value() << "," << start_layer << "->" << end_point.x().value() << "," << end_point.y().value() << "," << end_layer << std::endl;
            }*/
        }

        /*if (net_name == "net1192") {
         for (auto arc = graph_type::ArcIt(net_graph); arc != lemon::INVALID; ++arc) {
            std::cout << "arc " << net_graph.id(net_graph.source(arc)) << "," << net_graph.id(net_graph.target(arc)) << std::endl;
         }
        }*/

        return lemon::connected(net_graph);
    }

    bool GlobalRouting::move_cell(GlobalRouting::gcell_type source, GlobalRouting::gcell_type target, GlobalRouting::cell_instance_type cell, GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::routing_constraints_type & routing_constraints, GlobalRouting::std_cells_type & std_cells)
    {
        //remove blockage demand
        update_blockage_demand(netlist, placement, cell, true);
        //move cell from sets
        m_gcell_graph->remove_cell_instance(source, cell);
        m_gcell_graph->insert_cell_instance(target, cell);
        //change placement
        auto target_location = m_gcell_graph->center_of_box(target);
        placement.place(cell, target_location);
        //add blockage demand
        update_blockage_demand(netlist, placement, cell, false);
        incremental_update_extra_demand(source, netlist, placement, routing_constraints, std_cells);
        incremental_update_extra_demand(target, netlist, placement, routing_constraints, std_cells);
        auto overflow = overflow_movement(target);
        return overflow;
    }


    void GlobalRouting::restore_movement(GlobalRouting::gcell_type source,
                                         GlobalRouting::gcell_type target,
                                         GlobalRouting::cell_instance_type cell,
                                         GlobalRouting::netlist_type & netlist,
                                         GlobalRouting::placement_type & placement,
                                         GlobalRouting::routing_constraints_type & routing_constraints,
                                         GlobalRouting::std_cells_type & std_cells,
                                         std::vector<ophidian::routing::ExtraDemandGCell>& source_ex_demands,
                                         std::vector<ophidian::routing::ExtraDemandGCell>& target_ex_demands)
    {
        //remove blockage demand
        update_blockage_demand(netlist, placement, cell, true);
        //move cell from sets
        m_gcell_graph->remove_cell_instance(source, cell);
        m_gcell_graph->insert_cell_instance(target, cell);
        //change placement
        auto target_location = m_gcell_graph->center_of_box(target);
        placement.place(cell, target_location);
        //add blockage demand
        update_blockage_demand(netlist, placement, cell, false);
        for(auto extra_demand_gcell : source_ex_demands)
        {
            m_gcell_graph->clear_same_demand(extra_demand_gcell.gcell);
            m_gcell_graph->clear_adj_demand(extra_demand_gcell.gcell);
            m_gcell_graph->change_same_demand(extra_demand_gcell.gcell, extra_demand_gcell.same_demand);
            m_gcell_graph->change_adj_demand(extra_demand_gcell.gcell, extra_demand_gcell.adj_demand);
        }
        for(auto extra_demand_gcell : target_ex_demands)
        {
            m_gcell_graph->clear_same_demand(extra_demand_gcell.gcell);
            m_gcell_graph->clear_adj_demand(extra_demand_gcell.gcell);
            m_gcell_graph->change_same_demand(extra_demand_gcell.gcell, extra_demand_gcell.same_demand);
            m_gcell_graph->change_adj_demand(extra_demand_gcell.gcell, extra_demand_gcell.adj_demand);
        }
    }

    std::vector<ophidian::routing::ExtraDemandGCell> GlobalRouting::extra_demand_neighborhood(GlobalRouting::gcell_type gcell)
    {
        std::vector<ExtraDemandGCell> extra_demands;
        auto gcell_node = m_gcell_graph->graph_node(gcell);
        auto gcell_position = m_gcell_graph->position(gcell_node);
        for(auto layer_it = m_library.begin_layer(); layer_it != m_library.end_layer(); layer_it++)
        {
            auto layer_index = m_library.layerIndex(*layer_it);
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);
            ExtraDemandGCell affected;
            affected.gcell = affected_gcell;
            affected.same_demand = m_gcell_graph->same_demand(affected_gcell);
            affected.adj_demand = m_gcell_graph->adj_demand(affected_gcell);
            extra_demands.push_back(affected);

            auto east_node = m_gcell_graph->east_node(affected_node);
            if(east_node != lemon::INVALID)
            {
                auto east_gcell = m_gcell_graph->gcell(east_node);
                ExtraDemandGCell east;
                east.gcell = east_gcell;
                east.same_demand = m_gcell_graph->same_demand(east_gcell);
                east.adj_demand = m_gcell_graph->adj_demand(east_gcell);
                extra_demands.push_back(east);
            }
            auto west_node = m_gcell_graph->west_node(affected_node);
            if(west_node != lemon::INVALID)
            {
                auto west_gcell = m_gcell_graph->gcell(west_node);
                ExtraDemandGCell west;
                west.gcell = west_gcell;
                west.same_demand = m_gcell_graph->same_demand(west_gcell);
                west.adj_demand = m_gcell_graph->adj_demand(west_gcell);
                extra_demands.push_back(west);
            }
        }
        return extra_demands;
    }

    bool GlobalRouting::overflow_movement(GlobalRouting::gcell_type gcell)
    {
        auto gcell_node = m_gcell_graph->graph_node(gcell);
        auto gcell_position = m_gcell_graph->position(gcell_node);
        for(auto layer_it = m_library.begin_layer(); layer_it != m_library.end_layer(); layer_it++)
        {
            auto layer_index = m_library.layerIndex(*layer_it);
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);

            if(m_gcell_graph->is_overflow(affected_gcell))
                return true;
            auto east_node = m_gcell_graph->east_node(affected_node);
            auto east_gcell = m_gcell_graph->gcell(east_node);
            auto west_node = m_gcell_graph->west_node(affected_node);
            auto west_gcell = m_gcell_graph->gcell(west_node);
            if (east_node != lemon::INVALID)
                if(m_gcell_graph->is_overflow(east_gcell))
                    return true;
            if (west_node != lemon::INVALID)
                if(m_gcell_graph->is_overflow(west_gcell))
                    return true;
        }
        return false;
    }

    void GlobalRouting::update_blockage_demand(GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::cell_instance_type cell, bool remove_demand)
    {
        auto location = placement.location(cell);
        auto std_cell = netlist.std_cell(cell);
        for(auto blockage : m_library.blockages(std_cell))
        {
            auto layer = m_library.layer(blockage);
            auto layer_index = m_library.layerIndex(layer);
            auto gcell = m_gcell_graph->nearest_gcell(location, layer_index-1);
            auto demand = remove_demand ? -m_library.demand(blockage) : m_library.demand(blockage);
            m_gcell_graph->change_blockage_demand(gcell, demand);
        }
    }

    void GlobalRouting::update_extra_demand(GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::routing_constraints_type & routing_constraints, GlobalRouting::std_cells_type & std_cells)
    {
        for(auto gcell_it = m_gcell_graph->begin_gcell(); gcell_it != m_gcell_graph->end_gcell(); gcell_it++){
            auto gcell = *gcell_it;
            m_gcell_graph->clear_same_demand(gcell);
            m_gcell_graph->clear_adj_demand(gcell);
        }

        std::unordered_map<gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        std::unordered_map<circuit::StandardCells::cell_type, std::unordered_set<gcell_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> gcells_per_std_cell;

        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++)
        {
            auto cell = *cell_it;
            auto location = placement.location(cell);
            auto std_cell = netlist.std_cell(cell);

            auto cell_name = netlist.name(cell);
            auto std_cell_name = std_cells.name(std_cell);

            for (auto layer_index = m_library.lowest_layer_index(); layer_index <= m_library.highest_layer_index(); layer_index++)
            {
                auto gcell = m_gcell_graph->nearest_gcell(location, layer_index-1);

                std_cells_per_gcell[gcell][std_cell] += 1;
                gcells_per_std_cell[std_cell].insert(gcell);
            }
        }

        for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++)
        {
            auto key = same_grid_it->first;
            auto & extra_demand = same_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = std_cells.find_cell(macro1_name);
            auto macro2 = std_cells.find_cell(macro2_name);
            auto layer = m_library.find_layer_instance(layer_name);

            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];

            auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1)
            {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2)
            {
                rule_gcells.insert(gcell);
            }

            for (auto gcell : rule_gcells)
            {
                auto gcell_layer_index = m_gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_library.find_layer_instance(gcell_layer_name);

                if (layer == gcell_layer)
                {
                    auto pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[gcell][macro2]);
                    auto delta = pair_count * demand;
                    m_gcell_graph->change_same_demand(gcell, delta);
                }
            }
        }

        for (auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++)
        {
            auto key = adj_grid_it->first;
            auto & extra_demand = adj_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = std_cells.find_cell(macro1_name);
            auto macro2 = std_cells.find_cell(macro2_name);
            auto layer = m_library.find_layer_instance(layer_name);

            auto gcells_macro1 = gcells_per_std_cell[macro1];
            auto gcells_macro2 = gcells_per_std_cell[macro2];

            auto rule_gcells = std::unordered_set<gcell_type, entity_system::EntityBaseHash>{};
            for (auto gcell : gcells_macro1)
            {
                rule_gcells.insert(gcell);
            }
            for (auto gcell : gcells_macro2)
            {
                rule_gcells.insert(gcell);
            }

            for (auto gcell : rule_gcells)
            {
                auto gcell_node = m_gcell_graph->graph_node(gcell);
                auto east_node = m_gcell_graph->east_node(gcell_node);
                auto east_gcell = m_gcell_graph->gcell(east_node);
                auto west_node = m_gcell_graph->west_node(gcell_node);
                auto west_gcell = m_gcell_graph->gcell(west_node);

                auto gcell_layer_index = m_gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_library.find_layer_instance(gcell_layer_name); if (layer == gcell_layer) { auto east_pair_count = 0;
                    auto west_pair_count = 0;
                    if (macro1 == macro2)
                    {
                        if (east_node != lemon::INVALID)
                        {
                            east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro1]);
                        }
                        if (west_node != lemon::INVALID)
                        {
                            west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro1]);
                        }
                    }
                    else
                    {
                        if (east_node != lemon::INVALID)
                        {
                            east_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[east_gcell][macro2])
                                + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[east_gcell][macro1]);
                        }
                        if (west_node != lemon::INVALID)
                        {
                            west_pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[west_gcell][macro2])
                                + std::min(std_cells_per_gcell[gcell][macro2], std_cells_per_gcell[west_gcell][macro1]);
                        }
                    }
                    auto delta = (east_pair_count + west_pair_count) * demand;
                    m_gcell_graph->change_adj_demand(gcell, delta);
                }
            }
        }
    }

    void GlobalRouting::incremental_update_extra_demand(GlobalRouting::gcell_type gcell, GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::routing_constraints_type & routing_constraints, GlobalRouting::std_cells_type & std_cells)
    {
        update_same_extra_demand(gcell, netlist, placement, routing_constraints, std_cells);
        update_adj_extra_demand(gcell, netlist, placement, routing_constraints, std_cells);

        //Update potential affected gcells adj extra demand
        auto node = m_gcell_graph->graph_node(gcell);
        auto east_node = m_gcell_graph->east_node(node);
        if(east_node != lemon::INVALID)
            update_adj_extra_demand(m_gcell_graph->gcell(east_node), netlist, placement, routing_constraints, std_cells);
        auto west_node = m_gcell_graph->west_node(node);
        if(west_node != lemon::INVALID)
            update_adj_extra_demand(m_gcell_graph->gcell(west_node), netlist, placement, routing_constraints, std_cells);
    }

    void GlobalRouting::update_same_extra_demand(GlobalRouting::gcell_type gcell, GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::routing_constraints_type & routing_constraints, GlobalRouting::std_cells_type & std_cells)
    {
        //Clear current same extra demand
        auto node = m_gcell_graph->graph_node(gcell);
        auto gcell_position = m_gcell_graph->position(node);
        for(auto layer_it = m_library.begin_layer(); layer_it != m_library.end_layer(); layer_it++)
        {
            auto layer_index = m_library.layerIndex(*layer_it);
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);
            m_gcell_graph->clear_same_demand(affected_gcell);
        }

        //Count macros from gcells
        std::unordered_map<gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        for(auto cell_it = m_gcell_graph->cell_instances(gcell).begin(); cell_it != m_gcell_graph->cell_instances(gcell).end(); cell_it++)
        {
            auto std_cell = netlist.std_cell(*cell_it);
            std_cells_per_gcell[gcell][std_cell] += 1;
        }
        for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++)
        {
            auto key = same_grid_it->first;
            auto & extra_demand = same_grid_it->second;
            auto demand = extra_demand.demand;
            auto layer_name = extra_demand.layer;
            auto layer = m_library.find_layer_instance(layer_name);
            auto layer_index = m_library.layerIndex(layer);
            auto macro1 = std_cells.find_cell(extra_demand.macro1);
            auto macro2 = std_cells.find_cell(extra_demand.macro2);
            int macro1_count = std_cells_per_gcell[gcell].find(macro1) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro1];
            int macro2_count = std_cells_per_gcell[gcell].find(macro2) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro2];
            int pair_count = std::min(macro1_count, macro2_count);
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);
            auto delta = pair_count * demand;
            m_gcell_graph->change_same_demand(affected_gcell, delta);
        }
    }

    void GlobalRouting::update_adj_extra_demand(GlobalRouting::gcell_type gcell, GlobalRouting::netlist_type & netlist, GlobalRouting::placement_type & placement, GlobalRouting::routing_constraints_type & routing_constraints, GlobalRouting::std_cells_type & std_cells)
    {
        //Clear current adj extra demand
        auto node = m_gcell_graph->graph_node(gcell);
        auto gcell_position = m_gcell_graph->position(node);
        for(auto layer_it = m_library.begin_layer(); layer_it != m_library.end_layer(); layer_it++)
        {
            auto layer_index = m_library.layerIndex(*layer_it);
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);
            m_gcell_graph->clear_adj_demand(affected_gcell);
        }

        //Count macros from adj gcells
        std::unordered_map<gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        std::unordered_set<gcell_type, entity_system::EntityBaseHash> gcells_to_count{gcell};
        auto east_node = m_gcell_graph->east_node(node);
        auto east_gcell = m_gcell_graph->gcell(east_node);
        if(east_node != lemon::INVALID)
            gcells_to_count.insert(m_gcell_graph->gcell(east_node));
        auto west_node = m_gcell_graph->west_node(node);
        auto west_gcell = m_gcell_graph->gcell(west_node);
        if(west_node != lemon::INVALID)
            gcells_to_count.insert(m_gcell_graph->gcell(west_node));
        for(auto gcell : gcells_to_count)
        {
            for(auto cell_it = m_gcell_graph->cell_instances(gcell).begin(); cell_it != m_gcell_graph->cell_instances(gcell).end(); cell_it++)
            {
                auto std_cell = netlist.std_cell(*cell_it);
                std_cells_per_gcell[gcell][std_cell] += 1;
            }
        }

        //Adjacent GCell extra demand
        for (auto adj_grid_it = routing_constraints.begin_adj_grid(); adj_grid_it != routing_constraints.end_adj_grid(); adj_grid_it++)
        {
            auto key = adj_grid_it->first;
            auto & extra_demand = adj_grid_it->second;
            auto demand = extra_demand.demand;
            auto layer_name = extra_demand.layer;
            auto macro1 = std_cells.find_cell(extra_demand.macro1);
            auto macro2 = std_cells.find_cell(extra_demand.macro2);
            auto layer = m_library.find_layer_instance(layer_name);
            auto layer_index = m_library.layerIndex(layer);

            auto east_pair_count = 0;
            auto west_pair_count = 0;
            if (macro1 == macro2)
            {
                int gcell_count = std_cells_per_gcell[gcell].find(macro1) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro1];
                if (east_node != lemon::INVALID)
                {
                    int east_count = std_cells_per_gcell[east_gcell].find(macro2) == std_cells_per_gcell[east_gcell].end() ? 0 : std_cells_per_gcell[east_gcell][macro2];
                    east_pair_count = std::min(gcell_count, east_count);
                }
                if (west_node != lemon::INVALID)
                {
                    int west_count = std_cells_per_gcell[west_gcell].find(macro2) == std_cells_per_gcell[west_gcell].end() ? 0 : std_cells_per_gcell[west_gcell][macro2];
                    west_pair_count = std::min(gcell_count, west_count);
                }
            }
            else
            {
                int gcell_count_macro1 = std_cells_per_gcell[gcell].find(macro1) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro1];
                int gcell_count_macro2 = std_cells_per_gcell[gcell].find(macro2) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro2];
                if (east_node != lemon::INVALID)
                {
                    int east_count_macro1 = std_cells_per_gcell[east_gcell].find(macro1) == std_cells_per_gcell[east_gcell].end() ? 0 : std_cells_per_gcell[east_gcell][macro1];
                    int east_count_macro2 = std_cells_per_gcell[east_gcell].find(macro2) == std_cells_per_gcell[east_gcell].end() ? 0 : std_cells_per_gcell[east_gcell][macro2];
                    east_pair_count = std::min(gcell_count_macro1, east_count_macro2) + std::min(gcell_count_macro2, east_count_macro1);
                }
                if (west_node != lemon::INVALID)
                {
                    int west_count_macro1 = std_cells_per_gcell[west_gcell].find(macro1) == std_cells_per_gcell[west_gcell].end() ? 0 : std_cells_per_gcell[west_gcell][macro1];
                    int west_count_macro2 = std_cells_per_gcell[west_gcell].find(macro2) == std_cells_per_gcell[west_gcell].end() ? 0 : std_cells_per_gcell[west_gcell][macro2];
                    west_pair_count = std::min(gcell_count_macro1, west_count_macro2) + std::min(gcell_count_macro2, west_count_macro1);
                }
            }
            auto affected_node = m_gcell_graph->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = m_gcell_graph->gcell(affected_node);
            auto delta = (east_pair_count + west_pair_count) * demand;
            m_gcell_graph->change_adj_demand(affected_gcell, delta);
        }
    }
}
