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
        if( def.gcell_x_axis().size() != 0 && def.gcell_y_axis().size() != 0)
        {
            int z = library.index(library.highest_layer());
            globalRouting.create_gcell_graph(library, def.gcell_x_axis(), def.gcell_y_axis(), z);
        }
        else
        {
            //TODO: from a .guide, construct a GCellGraph following .guide coordinates.
            std::cout << "WARNING : This Design DOES NOT have gcell definitions in .def file!" << std::endl;
            std::cout << "        : GCellGraph WILL NOT be initialized." << std::endl;
        }

        /*for(auto net : guide.nets())
        {
            auto net_instance = netlist.find_net(net.name());
            for(auto region : net.regions())
            {
                auto layer_instance = library.find_layer_instance(region.metal());
                globalRouting.add_segment(region.region(), layer_instance, layer_instance, net_instance);
            }
        }*/
    }

    void make_global_routing(GlobalRouting& globalRouting, const Library & library, const RoutingConstraints & routing_constraints, const ophidian::circuit::Netlist & netlist, const ophidian::circuit::StandardCells & std_cells, const ophidian::placement::Placement & placement, const ophidian::parser::ICCAD2020 & iccad_2020) noexcept
    {
        //Create GCell Graph
        auto dimensions = iccad_2020.grid_dimensions();
        auto gcell_size = 10.0;
        ophidian::routing::GCellGraph::unit_container_type x_gcell_axis, y_gcell_axis;
        x_gcell_axis.reserve(std::get<0>(dimensions)+1);
        y_gcell_axis.reserve(std::get<1>(dimensions)+1);
        for(int x = 0; x <= std::get<0>(dimensions); x++)
            x_gcell_axis.push_back(util::micrometer_t{x*gcell_size});
        for(int y = 0; y <= std::get<1>(dimensions); y++)
            y_gcell_axis.push_back(util::micrometer_t{y*gcell_size});
        auto iccad_layers = iccad_2020.layers();
        std::vector<int> layers_capacity;
        layers_capacity.resize(iccad_layers.size());
        for(auto layer : iccad_layers)
            layers_capacity.at(layer.index()-1) = layer.capacity();
        // std::cout << "create routing graph" << std::endl;
        globalRouting.create_gcell_graph(x_gcell_axis, y_gcell_axis, std::get<2>(dimensions), layers_capacity);
        // std::cout << "done" << std::endl;

        auto gcell_graph = globalRouting.gcell_graph();
        
        // std::cout << "set gcell capacities" << std::endl;
        for (auto gcell_it = gcell_graph->begin_gcell(); gcell_it != gcell_graph->end_gcell(); gcell_it++) {
            auto gcell = *gcell_it;
            auto gcell_node = gcell_graph->graph_node(gcell);
            auto gcell_position = gcell_graph->position(gcell_node);

            auto gcell_capacity = gcell_graph->capacity(gcell);
            auto non_default_supply = routing_constraints.ndf_constraint(gcell_position.get<0>(), gcell_position.get<1>(), gcell_position.get<2>());
            // std::cout << "gcell " << gcell_position.get<0>() << "," << gcell_position.get<1>() << "," << gcell_position.get<2>() << " non default supply " << non_default_supply << " new capacity " << (gcell_capacity + non_default_supply) << std::endl;
            gcell_graph->capacity(gcell, gcell_capacity + non_default_supply);
        }
        
        // std::cout << "add blockages" << std::endl;
        //set cells demands in gcell_grapg
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++)
        {
            auto cell = *cell_it;
            auto location = placement.location(cell);
            auto std_cell = netlist.std_cell(cell);
            auto blockages = library.blockages(std_cell);
            for(auto blockage : blockages)
            {
                auto layer = library.layer(blockage);
                auto layer_index = library.layerIndex(layer);
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);
                auto demand = library.demand(blockage);
                gcell_graph->change_blockage_demand(gcell, demand);
            }
        }

        // std::cout << "add segments" << std::endl;
        //Global Routing Segments
        for(auto iccad_net : iccad_2020.nets())
        {
            auto net_name = iccad_net.name();
            auto segments = iccad_2020.segments(net_name);
            auto net = netlist.find_net(net_name);
            for(auto segment : segments)
            {
                auto start = segment.first;
                auto end = segment.second;
                auto gcell1 = gcell_graph->gcell(std::get<0>(start)-1, std::get<1>(start)-1, std::get<2>(start)-1);
                auto gcell2 = gcell_graph->gcell(std::get<0>(end)-1, std::get<1>(end)-1, std::get<2>(end)-1);
                auto box1 = gcell_graph->box(gcell1);
                auto box2 = gcell_graph->box(gcell2);
                auto box1_center_x = (box1.min_corner().x() + box1.max_corner().x()) / 2.0;
                auto box1_center_y = (box1.min_corner().y() + box1.max_corner().y()) / 2.0;
                auto box2_center_x = (box2.min_corner().x() + box2.max_corner().x()) / 2.0;
                auto box2_center_y = (box2.min_corner().y() + box2.max_corner().y()) / 2.0;
                auto min_x = std::min(box1_center_x, box2_center_x);
                auto min_y = std::min(box1_center_y, box2_center_y);
                auto max_x = std::max(box1_center_x, box2_center_x);
                auto max_y = std::max(box1_center_y, box2_center_y);
                ophidian::routing::GlobalRouting::segment_geometry_type box_segment{{min_x, min_y}, {max_x, max_y}};
                //get layer by index
                auto layer_start_it = std::find_if(library.begin_layer(),
                                                   library.end_layer(),
                                                   [&](auto & layer){return library.layerIndex(layer) == std::get<2>(start);});
                //get layer by index
                auto layer_end_it = std::find_if(library.begin_layer(),
                                                 library.end_layer(),
                                                 [&](auto & layer){return library.layerIndex(layer) == std::get<2>(end);});

                globalRouting.add_segment(box_segment, *layer_start_it, *layer_end_it, net);
            }
            /*
                For one net, no matter how much routing segments crossing one gGrid (GCell),
                it will only consume one demand in that gGrid.
            */
            globalRouting.increase_demand(net);
        }
    }
}
