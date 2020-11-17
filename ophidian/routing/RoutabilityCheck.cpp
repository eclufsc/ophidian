#include "RoutabilityCheck.h"
#include <ophidian/util/log.h>

bool ophidian::routing::check_connectivity(const ophidian::design::Design & design, const std::vector<ophidian::circuit::Net>& nets){
    using namespace ophidian::util;
    log() << "Checking for disconnected nets..." << std::endl;
    bool is_nets_open = false;
    int count = 0;
    for (auto net : nets) {

        auto net_name = design.netlist().name(net);
        // uncomment to discart Pin1 Nets (large number of pins)
        // if(net_name == "pin1")
            // continue;

        auto net_pins = design.netlist().pins(net);
        if (net_pins.size() < 2) {
            continue;
        }


        auto segments_connected = segments_are_connected(design, net);
        auto pins_connected = pins_are_connected(design, net);       
        auto connected = segments_connected && pins_connected;
        // auto connected = segments_are_connected(design, net);

        //log() << "segments connected " << segments_connected << " pins connected " << pins_connected << std::endl;

        if(!connected)
        {

            log() << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
            count ++;
            is_nets_open = true;
        }
    }
    if (is_nets_open) printflog("Open nets detected! %d nets are open!\n", count); else printlog("All nets connected!");
    return is_nets_open;
}

bool ophidian::routing::pins_are_connected(const ophidian::design::Design & design, const ophidian::circuit::Net & net){
    namespace bgi = boost::geometry::index;
    using box_scalar_type       = ophidian::geometry::Box<double>;
    using rtree_node_type       = std::pair<box_scalar_type, ophidian::routing::GlobalRouting::gr_segment_type>;
    using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16> >;
    std::unordered_map<int, rtree_type> rtree_layers;

    bool is_nets_open = false;
    auto & global_routing = design.global_routing();

    for (auto segment : global_routing.segments(net)) {
        
        auto gcell_start = global_routing.gcell_start(segment);
        auto gcell_end = global_routing.gcell_end(segment);

        auto layer_start = global_routing.gcell_graph()->layer_index(gcell_start);
        auto layer_end = global_routing.gcell_graph()->layer_index(gcell_end);

        auto gcell_start_box = global_routing.gcell_graph()->box(gcell_start);
        auto gcell_end_box = global_routing.gcell_graph()->box(gcell_end);

        //std::cout << "gcell start box " << gcell_start_box.min_corner().x().value() << "," << gcell_start_box.min_corner().y().value() << "->" << gcell_start_box.max_corner().x().value() << "," << gcell_start_box.max_corner().y().value() << std::endl;
        //std::cout << "gcell end box " << gcell_end_box.min_corner().x().value() << "," << gcell_end_box.min_corner().y().value() << "->" << gcell_end_box.max_corner().x().value() << "," << gcell_end_box.max_corner().y().value() << std::endl;
        
        if(layer_start == layer_end){
            //segment
            std::vector<double> x_cords {gcell_start_box.min_corner().x().value(), gcell_start_box.max_corner().x().value(), gcell_end_box.min_corner().x().value(), gcell_end_box.max_corner().x().value()};
            std::vector<double> y_cords {gcell_start_box.min_corner().y().value(), gcell_start_box.max_corner().y().value(), gcell_end_box.min_corner().y().value(), gcell_end_box.max_corner().y().value()};
           
            auto min_x = x_cords.at(std::distance(x_cords.begin(), std::min_element(x_cords.begin(), x_cords.end())));
            auto min_y = y_cords.at(std::distance(y_cords.begin(), std::min_element(y_cords.begin(), y_cords.end())));
            auto max_x = x_cords.at(std::distance(x_cords.begin(), std::max_element(x_cords.begin(), x_cords.end())));
            auto max_y = y_cords.at(std::distance(y_cords.begin(), std::max_element(y_cords.begin(), y_cords.end())));

            auto gcell_scalar_box = box_scalar_type{{min_x, min_y}, {max_x, max_y}};
            auto node_start = rtree_node_type{gcell_scalar_box, segment};
            rtree_layers[layer_start].insert(node_start);

        }else{
            // via
            auto gcell_start_scalar_box = box_scalar_type{{gcell_start_box.min_corner().x().value(), gcell_start_box.min_corner().y().value()}, {gcell_start_box.max_corner().x().value(), gcell_start_box.max_corner().y().value()}};
            auto gcell_end_scalar_box = box_scalar_type{{gcell_end_box.min_corner().x().value(), gcell_end_box.min_corner().y().value()}, {gcell_end_box.max_corner().x().value(), gcell_end_box.max_corner().y().value()}};

            auto node_start = rtree_node_type{gcell_start_scalar_box, segment};
            auto node_end = rtree_node_type{gcell_end_scalar_box, segment};

            rtree_layers[layer_start].insert(node_start);
            rtree_layers[layer_end].insert(node_end);
        }
    }

    auto net_name = design.netlist().name(net);
    ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
    auto pins_connected = true;
    for (auto pin : design.netlist().pins(net)) {
        auto pin_name = design.netlist().name(pin);
        //std::cout << "pin " << pin_name << std::endl;
        auto pin_connected = false;
        for (auto pin_box : design.placement().geometry(pin)) {
            auto box_layer_name = pin_box.second;
            auto box_layer = design.routing_library().find_layer_instance(box_layer_name);
            auto box_layer_index = design.routing_library().layerIndex(box_layer);

            auto pin_box_scalar = box_scalar_type{{pin_box.first.min_corner().x().value(), pin_box.first.min_corner().y().value()}, {pin_box.first.max_corner().x().value(), pin_box.first.max_corner().y().value()}};

            std::vector<rtree_node_type> intersecting_nodes;
            rtree_layers[box_layer_index].query(boost::geometry::index::intersects(pin_box_scalar), std::back_inserter(intersecting_nodes));

            //std::cout << "box " << pin_box_scalar.min_corner().x() << "," << pin_box_scalar.min_corner().x() << "-> " << pin_box_scalar.max_corner().x() << "," << pin_box_scalar.max_corner().y() << std::endl;
            //std::cout << "connected " << !intersecting_nodes.empty() << std::endl;

            pin_connected |= !intersecting_nodes.empty();
        }
        pins_connected &= pin_connected;
    }
    
    return pins_connected;
}

bool ophidian::routing::segments_are_connected(const ophidian::design::Design & design, const ophidian::circuit::Net & net){
    namespace bgi = boost::geometry::index;
    using scalar_type           = int;
    using point_scalar_type     = ophidian::geometry::Point<double>;
    using box_scalar_type       = ophidian::geometry::Box<double>;
    using unity_type            = GlobalRouting::unit_type;
    using point_type            = ophidian::geometry::Point<unity_type>;
    using box_type              = geometry::Box<unity_type>;
    using gcell_type            = GCell;
    using rtree_node_type       = std::pair<box_scalar_type, gcell_type>;
    using rtree_type            = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16> >;
    std::unordered_map<scalar_type, rtree_type> rtree_layers;
    using graph_type            = lemon::ListGraph;
    using node_type             = graph_type::Node;       
    using edge_type             = graph_type::Edge;

    //std::cout << "segments are connected " << std::endl;

    auto & global_routing = design.global_routing();

    auto net_graph = graph_type{};
    std::unordered_map<gcell_type, node_type, entity_system::EntityBaseHash> gcell_to_node;

    for (auto segment : global_routing.segments(net)) {
        auto segment_box =  global_routing.box(segment); 
        auto segment_min_corner = segment_box.min_corner();
        auto segment_max_corner = segment_box.max_corner();
        auto segment_scalar_box = box_scalar_type{{segment_min_corner.x().value(), segment_min_corner.y().value()}, {segment_max_corner.x().value(), segment_max_corner.y().value()}};
        
        auto gcell_start = global_routing.gcell_start(segment); 
        auto gcell_end = global_routing.gcell_end(segment);
        auto start_point = global_routing.gcell_graph()->center_of_box(gcell_start);
        auto start_layer =  global_routing.gcell_graph()->layer_index(gcell_start);
        auto end_point = global_routing.gcell_graph()->center_of_box(gcell_end);
        auto end_layer = global_routing.gcell_graph()->layer_index(gcell_end);

        //std::cout << "segment " << start_point.x().value() << "," << start_point.y().value() << "," << start_layer << "->" << end_point.x().value() << "," << end_point.y().value() << "," << end_layer << std::endl;             
        //std::cout << "segment " << segment_box.min_corner().x().value() << "," << segment_box.min_corner().y().value() << "," << start_layer << "->" << segment_box.max_corner().x().value() << "," << segment_box.max_corner().y().value() << "," << end_layer << std::endl;             

        auto start_node = node_type{};
        if (gcell_to_node.find(gcell_start) == gcell_to_node.end()) {
            start_node = net_graph.addNode();
            gcell_to_node[gcell_start] = start_node;
        } else {
            start_node = gcell_to_node[gcell_start];
        }
        
        std::vector<rtree_node_type> intersecting_nodes;
        rtree_layers[start_layer].query(boost::geometry::index::intersects(segment_scalar_box), std::back_inserter(intersecting_nodes));
        for (auto node : intersecting_nodes) {
            auto node_box = node.first;
            //std::cout << "connected to start node " << node_box.min_corner().x() << "," << node_box.min_corner().y() << "->" << node_box.max_corner().x() << "," << node_box.max_corner().y() << std::endl;
            auto graph_node = gcell_to_node[node.second];
            if (graph_node != start_node) {
                net_graph.addEdge(start_node, graph_node);
            }
        }

        auto end_node = node_type{};
        if (gcell_to_node.find(gcell_end) == gcell_to_node.end()) {
            end_node = net_graph.addNode();
            gcell_to_node[gcell_end] = end_node;
        } else {
            end_node = gcell_to_node[gcell_end];
        }
        net_graph.addEdge(start_node, end_node);
        
        if (start_layer != end_layer) {
            std::vector<rtree_node_type> intersecting_nodes;
            rtree_layers[end_layer].query(boost::geometry::index::intersects(segment_scalar_box), std::back_inserter(intersecting_nodes));
            for (auto node : intersecting_nodes) {
                auto node_box = node.first;
                //std::cout << "connected to end node " << node_box.min_corner().x() << "," << node_box.min_corner().y() << "->" << node_box.max_corner().x() << "," << node_box.max_corner().y() << std::endl;
                auto graph_node = gcell_to_node[node.second];
                if (graph_node != end_node) {
                    net_graph.addEdge(end_node, graph_node);
                }
            }
            auto gcell_start_box = global_routing.gcell_graph()->box(gcell_start);
            auto gcell_start_scalar_box = box_scalar_type{{gcell_start_box.min_corner().x().value(), gcell_start_box.min_corner().y().value()}, {gcell_start_box.max_corner().x().value(), gcell_start_box.max_corner().y().value()}};
            auto start_rtree_node = rtree_node_type{gcell_start_scalar_box, gcell_start};
            rtree_layers[start_layer].insert(start_rtree_node);
            auto gcell_end_box = global_routing.gcell_graph()->box(gcell_end);
            auto gcell_end_scalar_box = box_scalar_type{{gcell_end_box.min_corner().x().value(), gcell_end_box.min_corner().y().value()}, {gcell_end_box.max_corner().x().value(), gcell_end_box.max_corner().y().value()}};
            auto end_rtree_node = rtree_node_type{gcell_end_scalar_box, gcell_end};
            rtree_layers[end_layer].insert(end_rtree_node);
        } else {
            auto rtree_node = rtree_node_type{segment_scalar_box, gcell_start};
            rtree_layers[start_layer].insert(rtree_node);
        }

    }

    return lemon::connected(net_graph);
}
