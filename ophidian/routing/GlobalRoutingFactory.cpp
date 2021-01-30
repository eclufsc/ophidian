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
        namespace bgi = boost::geometry::index;
        using point_scalar_type     = ophidian::geometry::Point<double>;
        using box_scalar_type       = ophidian::geometry::Box<double>;
        using unity_type            = GlobalRouting::unit_type;
        using point_type            = ophidian::geometry::Point<unity_type>;
        using box_type              = geometry::Box<unity_type>;
        using rtree_node_type       = std::pair<box_scalar_type, GlobalRouting::gr_segment_type>;
        using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16>>;
        std::map<GlobalRouting::scalar_type, rtree_type> rtree_layers;

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

        for(auto net : guide.nets())
        {
            auto net_instance = netlist.find_net(net.name());
            int min_layer = library.lowest_layer_index();
            int max_layer = library.highest_layer_index();

            //creating map for rtrees for layers
            rtree_layers.clear();
            for (int i = min_layer; i <= max_layer; i++)
            {
                rtree_layers.insert(std::make_pair(i, rtree_type()));
            }

            for(auto region : net.regions())
            {
                auto layer_instance = library.find_layer_instance(region.metal());
                auto index = library.index(layer_instance);
                auto segment = globalRouting.add_segment(region.region(), layer_instance, layer_instance, net_instance);
                //add segment in rtree
                auto region_box = region.region();
                auto min_corner = point_scalar_type(region_box.min_corner().x().value(), region_box.min_corner().y().value());
                auto max_corner = point_scalar_type(region_box.max_corner().x().value(), region_box.max_corner().y().value());
                auto box = box_scalar_type(min_corner, max_corner);
                rtree_layers.at(index).insert(std::make_pair(box, segment));
            }

            //add vias in intersections
            for (auto segment : globalRouting.segments(net_instance))
            {
                auto upper_layer = globalRouting.layer_start(segment);
                auto layer_indx = library.index(upper_layer);
                if(layer_indx == min_layer)
                    continue;
                auto lower_layer = library.layer_from_index(layer_indx -1);

                auto rtree = rtree_layers.at(layer_indx -1);
                auto region_box = globalRouting.box(segment);
                auto min_corner = point_scalar_type(region_box.min_corner().x().value(), region_box.min_corner().y().value());
                auto max_corner = point_scalar_type(region_box.max_corner().x().value(), region_box.max_corner().y().value());
                auto box = box_scalar_type(min_corner, max_corner);

                std::vector<rtree_node_type> result;
                rtree.query(bgi::intersects(box), std::back_inserter(result));
                for(auto r : result)
                {
                    auto node_segment = r.second;
                    auto node_box = r.first;

                    // this should insert a bug when the segment is just a line (without area)
                    box_scalar_type intersection;
                    boost::geometry::intersection(node_box, box, intersection);
                    if(boost::geometry::area(intersection) != 0 || boost::geometry::area(box) <= std::numeric_limits<double>::epsilon())
                    {
                        //creating a via
                        auto min_corner = point_type(unity_type(intersection.min_corner().x()), unity_type(intersection.min_corner().y()));
                        auto max_corner = point_type(unity_type(intersection.max_corner().x()), unity_type(intersection.max_corner().y()));
                        auto box = box_type(min_corner, max_corner);
                        globalRouting.add_segment(box, lower_layer, upper_layer, net_instance);
                    }
                }
            }
        }
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
            
            // It is optional have segments in the initial file when all pins of one net is in te same gcell.
            /*
                If there are no segments already routed, then
                create a single segment on top of the cell.
                This is simply so that the demand is corretcly counted.
            */
            if(segments.empty())
            {
                // assert i all pin's net are in the same gcell
                auto pin_it = netlist.pins(net).begin();
                auto position = placement.location(*pin_it);
                bool same_gcell = true;
                auto min_layer_idx = library.highest_layer_index(); 
                auto max_layer_idx = library.lowest_layer_index();
                for(auto pin : netlist.pins(net))
                {
                    if(placement.location(pin).x() != position.x() || placement.location(pin).y() != position.y())
                        same_gcell = false;
                    // get the minimum and maximum pin layers in this net
                    for(auto geometry : placement.geometry(pin))
                    {
                        auto layer_name = geometry.second;
                        auto layer = library.find_layer_instance(layer_name);
                        auto layer_index = library.index(layer);
                        if(layer_index > max_layer_idx)
                            max_layer_idx = layer_index;
                        if(layer_index < min_layer_idx)
                            min_layer_idx = layer_index;
                    }                    
                }
                if(!same_gcell)
                    std::cout << "NET " << netlist.name(net) << " NOT IN THE SAME GCELL" << std::endl;

                // get the minimul routing layer for this net
                auto net_min_layer = routing_constraints.min_net_layer(net);
                auto net_min_layer_idx = library.index(net_min_layer);
                if(net_min_layer_idx > max_layer_idx)
                    max_layer_idx = net_min_layer_idx;
                
                // contruct a segment od minimum layer to maximum layer
                auto location = placement.location(netlist.cell(*pin_it));
                auto min_x = location.x();
                auto min_y = location.y();
                auto max_x = min_x;
                auto max_y = min_y; 
                ophidian::routing::GlobalRouting::segment_geometry_type box_segment{{min_x, min_y}, {max_x, max_y}};

                //add segmento in global routing
                auto layer_start = library.layer_from_index(min_layer_idx);
                auto layer_end = library.layer_from_index(max_layer_idx);
                globalRouting.add_segment(box_segment, layer_start, layer_end, net);
            }
            
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
