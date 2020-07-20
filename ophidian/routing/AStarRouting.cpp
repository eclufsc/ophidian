#include <ophidian/routing/AStarRouting.h>
#include <queue>
#include <list>
#include <unordered_set>

bool AStarDebug = false;

namespace ophidian::routing
{

    enum class SegmentDirection {
        NA, X, Y, Z
    };

    struct comp
    {
        bool operator()(const AStarRouting::point_type& p1, const AStarRouting::point_type& p2) const
        {
            if (p1.x() != p2.x()) return p1.x() < p2.x();
            return p1.y() < p2.y();
        }
    };

    AStarRouting::AStarRouting(design_type & design):
        m_graph{},
        m_design{design},
        m_node_map{m_graph},
        m_edge_map{m_graph},
        m_gcell_graph{design.global_routing().gcell_graph()},
        m_gcell_to_AStarNode{design.global_routing().gcell_graph()->make_property_gcells(AStarNode{})},
        m_gcells_extra_demand{design.global_routing().gcell_graph()->make_property_gcells(int{0})}
    {
        update_extra_demand_constraint();
    }

    //when routing all nets is required first remove all of them
    bool AStarRouting::route_net(const AStarRouting::net_type & net)
    {
        m_net = net;

        //TODO: Save the current_routing as a backup

        auto& netlist = m_design.netlist();
        auto& routing_constraints = m_design.routing_constraints();
        auto net_size = netlist.pins(net).size();
        m_min_layer = routing_constraints.min_net_layer(net);

        bool all_pins_are_in_same_collumn = all_pins_same_collumn();
        if(all_pins_are_in_same_collumn == false)
        {
            //1-Call Flute.
            bool flute_graph;
            if(net_size > 2)
                flute_graph = init_flute_graph();
            else if(net_size == 2)
                flute_graph = init_two_pin_flute_graph();
            else//net only have 1 pin (For iccad2020 it should never happen)
                return false;

            if(flute_graph == false)
                return false;

            auto& global_routing = m_design.global_routing();
            global_routing.unroute(m_net);

            //2-Sort of layer assignment
            node_layer_assignment();
            //3-Connect each pair of nodes using A*.(consider min layer)
            route_flute_segments();
            //4-Convert gcell path into segments
            bfs_backtrack();
            //5-Connect cell pins to min layers
            connect_pins_to_min_layer();

            connect_floating_pins();
        }
        else
        {
            auto& global_routing = m_design.global_routing();
            global_routing.unroute(m_net);
            trivial_routing();
        }
        //6-Write segments in GlobalRouting
        write_routing_segments();
        //7-clean AStarRouting to be reused by other nets
        clear_router_members();
        //8-Clean up the code
        return true;
    }

    //return false when all cells are inside the same gcell
    bool AStarRouting::init_flute_graph()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        std::map<point_type, std::string, comp> pins_map;
        std::map<point_type, flute_node_type, comp> created_nodes;

        //Run Flute
        std::vector<interconnection::Flute::Point> net_points;
        auto net_pins = netlist.pins(m_net);
        // std::cout<<"Test: "<<net_pins.size()<<std::endl;
        net_points.reserve(netlist.pins(m_net).size());
        for(auto pin : net_pins)
        {
            auto pin_location = placement.location(pin);
            net_points.push_back(pin_location);
            auto pin_name = netlist.name(pin);
            pins_map[pin_location] = pin_name;

            auto cell = netlist.cell(pin);
            // std::cout<<"pin: "<<pin_name<<" cell: "<<netlist.name(cell)<<std::endl;
        }

        //std::cout<<"netsize: "<<net_pins.size()<<" map "<<pins_map.size()<<std::endl;
        auto & flute = interconnection::Flute::instance();
        // std::cout<<"calling flute: "<<net_points.size()<<std::endl;
        auto tree = flute.create(net_points);

        auto steiner_tree_length = tree->length().value();
        if(steiner_tree_length == 0)
        {
            std::cout<<"WARNING: same gcell"<<std::endl;
            std::cout<<"BUG in function: bool all_pins_same_collumn()"<<std::endl;
            return false;
        }

        //Create FluteGraph
        for(auto tree_segment_it = tree->segments().first; tree_segment_it != tree->segments().second; ++tree_segment_it)
        {
            // std::cout<<"tree is not empty"<<std::endl;
            auto tree_segment = *tree_segment_it;
            auto segment_start = tree->position(tree->u(tree_segment));
            auto segment_end = tree->position(tree->v(tree_segment));

            if (AStarDebug) std::cout << "tree segment " << segment_start.x().value() << "," << segment_start.y().value() << "->" << segment_end.x().value() << "," << segment_end.y().value() << std::endl;

            auto pin1_name = pins_map.find(segment_start) != pins_map.end() ? pins_map[segment_start] : "steiner";
            flute_node_type node1;
            if(created_nodes.count(segment_start))
                node1 = created_nodes[segment_start];
            else
            {
                node1 = m_graph.addNode();
                created_nodes[segment_start] = node1;
                m_node_map[node1] = FluteNode{segment_start, pin1_name};
            }

            auto pin2_name = pins_map.find(segment_end) != pins_map.end() ? pins_map[segment_end] : "steiner";
            flute_node_type node2;
            if(created_nodes.count(segment_end))
                node2 = created_nodes[segment_end];
            else
            {
                node2 = m_graph.addNode();
                created_nodes[segment_end] = node2;
                m_node_map[node2] = FluteNode{segment_end, pin2_name};
            }

            if (AStarDebug) std::cout << "node1: " << m_node_map[node1] << ", node2: " << m_node_map[node2] << std::endl;
            m_graph.addEdge(node1, node2);
        }
        return true;
    }

    //not actually using flute
    bool AStarRouting::init_two_pin_flute_graph()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();

        auto pin_1 = *netlist.pins(m_net).begin();
        auto p1_l = placement.location(pin_1);
        auto pin_2 = *std::next(netlist.pins(m_net).begin());
        auto p2_l = placement.location(pin_2);

        if(p1_l.x() == p2_l.x() && p1_l.y() == p2_l.y())
        {
            std::cout<<"WARNING: same gcell"<<std::endl;
            std::cout<<"BUG in function: bool all_pins_same_collumn()"<<std::endl;
            return false;
        }

        auto node1 = m_graph.addNode();
        m_node_map[node1] = FluteNode{p1_l, netlist.name(pin_1)};
        auto node2 = m_graph.addNode();
        m_node_map[node2] = FluteNode{p2_l, netlist.name(pin_2)};
        m_graph.addEdge(node1, node2);

        return true;
    }

    //This mapping is considering net min. layer constraint and gcell capacity
    void AStarRouting::node_layer_assignment()
    {
        auto& routing_library = m_design.routing_library();
        auto min_layer_index = routing_library.layerIndex(m_min_layer);
        auto& placement = m_design.placement();
        auto& netlist = m_design.netlist();
        //std::cout<<"mapping gcells"<<std::endl;
        for(flute_graph_type::NodeIt node(m_graph); node != lemon::INVALID; ++node)
        {
            auto& flute_node = m_node_map[node];
            int layer_index = min_layer_index;
            if(flute_node.pin_name != "steiner")//is a pin
            {
                auto pin = netlist.find_pin_instance(flute_node.pin_name);
                auto pin_geometry = placement.geometry(pin);
                auto pin_layer_name = pin_geometry.front().second;
                auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
                auto pin_layer_index = routing_library.layerIndex(pin_layer);
                layer_index = std::max(layer_index, pin_layer_index);//see: void connect_pins_to_min_layer();
            }
            auto gcell = m_gcell_graph->nearest_gcell(flute_node.point, layer_index-1);

            // auto s_node = m_gcell_graph->graph_node(gcell);
            // auto s_pos = m_gcell_graph->position(s_node);
            // std::cout<<"mapping node: "<<flute_node<<" into "<<" pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;

            //TODO: handle this checking the up node
            if(gcell_has_free_space(gcell))
                flute_node.mapped_gcell = gcell;
            else
            {
                //std::cout<<"WARNING: mapped node is overflowed!!!"<<std::endl;
                //expand the serch in BFS order until find a free gcell
                gcell = m_gcell_graph->nearest_gcell(flute_node.point, layer_index+2);//WORKAROUND solution!
                //TODO: check if the node is steiner or not!
                if(gcell_has_free_space(gcell) == false)
                {
                    if(flute_node.pin_name == "steiner")
                        std::cout<<"WARNING: steiner node mapped is overflowed!!!"<<std::endl;
                }
                else
                {
                    std::cout<<"WARNING: pin node mapped is overflowed!!!"<<std::endl;
                }
                flute_node.mapped_gcell = gcell;
            }
            /*
            auto pos = m_gcell_graph->position(m_gcell_graph->graph_node(flute_node.mapped_gcell));
            std::cout<<flute_node<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;
            */

            auto s_node = m_gcell_graph->graph_node(flute_node.mapped_gcell);
            auto s_pos = m_gcell_graph->position(s_node);
            // std::cout<<" again mapping node: "<<flute_node<<" into "<<" pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;
        }
    }

    //call a_star routing for each pair of flute nodes in BFS order from any node_pin
    void AStarRouting::route_flute_segments()
    {
        flute_graph_type::NodeMap<bool> visited_nodes{m_graph};
        for(flute_graph_type::NodeIt node(m_graph); node != lemon::INVALID; ++node)
        {
            // std::cout<<"test"<<std::endl;
            if(m_node_map[node].pin_name != "steiner")
            {
                m_root_node = node;
                break;
            }
        }
        // std::cout<<"root node: "<<m_node_map[m_root_node]<<std::endl;
        std::queue<flute_node_type> queue;
        queue.push(m_root_node);

        while(!queue.empty())
        {
            auto current_node = queue.front();
            queue.pop();
            visited_nodes[current_node] = true;
            //std::cout<<"visiting node: "<< m_node_map[current_node]<<std::endl;
            for(flute_graph_type::IncEdgeIt edge(m_graph, current_node); edge != lemon::INVALID; ++edge)
            {
                auto opposite_node = m_graph.oppositeNode(current_node, edge);
                if(visited_nodes[opposite_node] == false)
                {
                    queue.push(opposite_node);
                    a_star(current_node, opposite_node);
                }
            }
        }
    }

    //find a shortest path from start.mapped_gcell to goal.mapped_gcell
    void AStarRouting::a_star(flute_node_type start, flute_node_type goal)
    {
        bool goal_is_steiner = m_node_map[goal].pin_name == "steiner" ? true : false;
        std::list<gcell_type> open_nodes;
        open_nodes.push_back(m_node_map[start].mapped_gcell);
        std::vector<gcell_type> dirty_nodes;
        dirty_nodes.push_back(m_node_map[start].mapped_gcell);

        // code just for debug
        // auto node = m_gcell_graph->graph_node(m_node_map[start].mapped_gcell);
        // auto pos = m_gcell_graph->position(node);
        // std::cout<<m_node_map[start]<<std::endl;
        // std::cout<<"start node: "<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;
        // node = m_gcell_graph->graph_node(m_node_map[goal].mapped_gcell);
        // pos = m_gcell_graph->position(node);
        // std::cout<<m_node_map[goal]<<std::endl;
        // std::cout<<"goal node: "<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;
        // code just for debug

        gcell_type current_node;
        while(open_nodes.empty() == false)
        {
            current_node = open_nodes.front();
            if(goal_reached(current_node, m_node_map[goal].mapped_gcell, goal_is_steiner))
                break;
            open_nodes.pop_front();

            //discover neighbors
            auto current_neighbors = neighbors(current_node);
            //std::cout<<"neighbors size: "<<current_neighbors.size()<<std::endl;
            for(auto neighbor : current_neighbors)
            {
                update_f_score(current_node, neighbor, m_node_map[goal].mapped_gcell, goal_is_steiner);
                if(m_gcell_to_AStarNode[neighbor].discovered == false)
                {
                    open_nodes.push_back(neighbor);
                    m_gcell_to_AStarNode[neighbor].discovered = true;
                }
                dirty_nodes.push_back(neighbor);
            }
            //re-sort list
            open_nodes.sort([&](auto & gcell1, auto & gcell2)
            {
                auto& node1 = m_gcell_to_AStarNode[gcell1];
                auto& node2 = m_gcell_to_AStarNode[gcell2];
                    return node1.g + node1.h < node2.g + node2.h;
            });
            m_gcell_to_AStarNode[current_node].finished = true;
        }

        //TODO: return false (return AFTER clean dirty nodes)
        if(open_nodes.empty())
            std::cout<<"there is no path to the goal"<<std::endl;
        if(goal_is_steiner)
            m_node_map[goal].mapped_gcell = current_node;

        // std::cout<<"backtrack path result"<<std::endl;
        back_track_path(start, goal);

        //clean dirty nodes
        for(auto gcell : dirty_nodes)
        {
            auto& astar_node = m_gcell_to_AStarNode[gcell];
            astar_node.g = std::numeric_limits<unsigned int>::max();
            astar_node.h = 0;
            astar_node.discovered = false;
            astar_node.finished = false;
            astar_node.gcell_from = gcell_type{};
        }
    }

    //3D Manhattan distance, if goal is steiner point, disregard Z dimension.
    unsigned int AStarRouting::heuristic(const AStarRouting::gcell_type & source, const AStarRouting::gcell_type & goal, bool goal_is_steiner) const
    {
        auto source_position = m_gcell_graph->position(m_gcell_graph->graph_node(source));
        auto goal_position = m_gcell_graph->position(m_gcell_graph->graph_node(goal));
        unsigned int x_dist = std::abs(source_position.get<0>() - goal_position.get<0>());
        unsigned int y_dist = std::abs(source_position.get<1>() - goal_position.get<1>());
        unsigned int z_dist = goal_is_steiner ? 0 : std::abs(source_position.get<2>() - goal_position.get<2>());
        return x_dist + y_dist + z_dist;
    }

    //This method return the gncell_neighbors from a given gcell.
    //It is considering layer directons, min net constraint, gcell_graph boundaries, 
    // gcell capacities and if the node has already been finished by A* algorithm.
    //When a condition is not satisfied the violating gcell is removed from the result.
    std::vector<AStarRouting::gcell_type> AStarRouting::neighbors(AStarRouting::gcell_type gcell)
    {
        std::vector<AStarRouting::gcell_type> result;
        auto node = m_gcell_graph->graph_node(gcell);
        auto& routing_library = m_design.routing_library();
        auto min_layer_index = routing_library.layerIndex(m_min_layer)-1;
        auto layer_index = m_gcell_graph->layer_index(gcell);
        auto layer = routing_library.layer_from_index(layer_index);
        auto layer_direction = routing_library.direction(layer);

        // if(layer_direction == Direction::HORIZONTAL)
        //     std::cout<<routing_library.name(layer)<<" is horizontal"<<std::endl;
        // if(layer_direction == Direction::VERTICAL)
        //     std::cout<<routing_library.name(layer)<<" is vertical"<<std::endl;

        //down node
        auto down_node = m_gcell_graph->down_node(node);
        if(down_node != lemon::INVALID)//check for graph boundary
        {
            auto neighbor_gcell = m_gcell_graph->gcell(down_node);
            if(gcell_has_free_space(neighbor_gcell))
            {
                auto node_position = m_gcell_graph->position(down_node);
                if(node_position.get<2>() >= min_layer_index)//check for min layer constraint
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
        }

        //up node
        auto up_node = m_gcell_graph->up_node(node);
        if(up_node != lemon::INVALID)//check for graph boundary
        {
            auto neighbor_gcell = m_gcell_graph->gcell(up_node);
            if(gcell_has_free_space(neighbor_gcell))
                if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                    result.push_back(neighbor_gcell);
        }

        //get west and east nodes
        if(layer_direction == Direction::HORIZONTAL)
        {
            auto west_node = m_gcell_graph->west_node(node);
            if(west_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(west_node);
                if(gcell_has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
            auto east_node = m_gcell_graph->east_node(node);
            if(east_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(east_node);
                if(gcell_has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
        }
        else//get north and south nodes
        {
            auto north_node = m_gcell_graph->north_node(node);
            if(north_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(north_node);
                if(gcell_has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
            auto south_node = m_gcell_graph->south_node(node);
            if(south_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(south_node);
                if(gcell_has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
        }
        return result;
    }

    void AStarRouting::update_f_score(AStarRouting::gcell_type gcell, AStarRouting::gcell_type neighbor_gcell, AStarRouting::gcell_type goal_gcell, bool goal_is_steiner)
    {
        auto& neighbor_astar_node = m_gcell_to_AStarNode[neighbor_gcell];
        neighbor_astar_node.h = neighbor_astar_node.h == 0 ? heuristic(neighbor_gcell, goal_gcell, goal_is_steiner) : neighbor_astar_node.h;
        if(neighbor_astar_node.g >= m_gcell_to_AStarNode[gcell].g + 1)
        {
            neighbor_astar_node.g = m_gcell_to_AStarNode[gcell].g + 1;
            neighbor_astar_node.gcell_from = gcell;
        }
    }

    //when goal_gcell is a steiner point disregard Z dimension
    bool AStarRouting::goal_reached(const AStarRouting::gcell_type & source, const AStarRouting::gcell_type & goal, bool goal_is_steiner) const
    {
        auto source_node = m_gcell_graph->graph_node(source);
        auto source_pos = m_gcell_graph->position(source_node);
        auto goal_node = m_gcell_graph->graph_node(goal);
        auto goal_pos = m_gcell_graph->position(goal_node);
        return (source_pos.get<0>() == goal_pos.get<0>() &&
                source_pos.get<1>() == goal_pos.get<1>() &&
                (source_pos.get<2>() == goal_pos.get<2>() || goal_is_steiner));
    }

    //BFS printing
    void AStarRouting::bfs_backtrack()
    {
        flute_graph_type::NodeMap<bool> visited_nodes{m_graph};
        std::queue<flute_node_type> queue;
        queue.push(m_root_node);

        while(!queue.empty())
        {
            auto current_flute_node = queue.front();
            queue.pop();
            visited_nodes[current_flute_node] = true;
            for(flute_graph_type::IncEdgeIt edge(m_graph, current_flute_node); edge != lemon::INVALID; ++edge)
            {
                auto opposite_flute_node = m_graph.oppositeNode(current_flute_node, edge);
                if(visited_nodes[opposite_flute_node] == false)
                {
                    queue.push(opposite_flute_node);
                    gcell_path_to_routing_segments(current_flute_node, opposite_flute_node);
                }
            }
        }
    }

    //backtrack printing from two flute_nodes
    void AStarRouting::back_track_path(AStarRouting::flute_node_type s, AStarRouting::flute_node_type g)
    {
        std::vector<gcell_type> path;
        auto s_node = m_gcell_graph->graph_node(m_node_map[s].mapped_gcell);
        auto s_pos = m_gcell_graph->position(s_node);

        auto gcell = m_node_map[g].mapped_gcell;
        auto g_node = m_gcell_graph->graph_node(gcell);
        auto g_pos = m_gcell_graph->position(g_node);

        // std::cout<<"debugging net: "<<m_design.netlist().name(m_net)<<std::endl;
        // std::cout<<"backtrack from (G) "<<m_node_map[g]<<" to (S) "<<m_node_map[s]<<std::endl;
        // std::cout<<"S pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;
        // std::cout<<"G pos: ("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;

        bool non_stop = true;
        while(non_stop)
        {
            non_stop = (s_pos.get<0>() != g_pos.get<0>() || s_pos.get<1>() != g_pos.get<1>() || s_pos.get<2>() != g_pos.get<2>());
            // std::cout<<"("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;
            path.push_back(gcell);
            if(non_stop == false)
                break;
            gcell = m_gcell_to_AStarNode[gcell].gcell_from;
            g_node = m_gcell_graph->graph_node(gcell);
            g_pos = m_gcell_graph->position(g_node);
        }

        for(flute_graph_type::IncEdgeIt edge(m_graph, s); edge != lemon::INVALID; ++edge)
        {
            auto opposite_node = m_graph.oppositeNode(s, edge);
            if(opposite_node == g)
            {
                m_edge_map[edge] = path;
                break;
            }
        }
    }

    void AStarRouting::gcell_path_to_routing_segments(AStarRouting::flute_node_type s, AStarRouting::flute_node_type g)
    {
        std::vector<gcell_type> gcells;
        for(flute_graph_type::IncEdgeIt edge(m_graph, s); edge != lemon::INVALID; ++edge)
        {
            auto opposite_node = m_graph.oppositeNode(s, edge);
            if(opposite_node == g)
            {
                gcells = m_edge_map[edge];
                break;
            }
        }
        // std::cout<<"RESULT from (G) "<<m_node_map[g]<<" to (S) "<<m_node_map[s]<<std::endl;
        // for(auto gcell : gcells)
        // {
        //     auto g_node = m_gcell_graph->graph_node(gcell);
        //     auto g_pos = m_gcell_graph->position(g_node);
        //     std::cout<<"("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;
        // }

        //discover the direction of the segments
        std::vector<SegmentDirection> directions;
        directions.reserve(gcells.size());
        auto previous_gcell = gcells.front();
        for(auto current_gcell : gcells)
        {
            SegmentDirection direction = SegmentDirection::NA;
            auto current_pos = m_gcell_graph->position(m_gcell_graph->graph_node(current_gcell));
            auto previous_pos = m_gcell_graph->position(m_gcell_graph->graph_node(previous_gcell));
            direction = (current_pos.get<0>() - previous_pos.get<0>() != 0) ? SegmentDirection::X : direction;
            direction = (current_pos.get<1>() - previous_pos.get<1>() != 0) ? SegmentDirection::Y : direction;
            direction = (current_pos.get<2>() - previous_pos.get<2>() != 0) ? SegmentDirection::Z : direction;
            directions.push_back(direction);
            previous_gcell = current_gcell;
        }

        // for(auto direction : directions)
        // {
        //     if(direction == SegmentDirection::NA)
        //         std::cout<<"NA"<<std::endl;

        //     if(direction == SegmentDirection::X)
        //         std::cout<<"X"<<std::endl;

        //     if(direction == SegmentDirection::Y)
        //         std::cout<<"Y"<<std::endl;

        //     if(direction == SegmentDirection::Z)
        //         std::cout<<"Z"<<std::endl;
        // }

        //find routing segments
        auto direction_it = std::next(directions.begin());
        previous_gcell = gcells.front();
        gcell_type start_gcell = gcells.front();
        SegmentDirection previous_direction = *direction_it;
        //skipping first gcell, because it is not possible to get a direction from only one point.
        for(auto gcell_it = std::next(gcells.begin()); gcell_it != gcells.end(); gcell_it++)
        {
            if(previous_direction != *direction_it)
            {
                auto start_pos = m_gcell_graph->position(m_gcell_graph->graph_node(start_gcell));
                auto previous_pos = m_gcell_graph->position(m_gcell_graph->graph_node(previous_gcell));
                // std::cout<<"Segment ("<<start_pos.get<0>()<<","<<start_pos.get<1>()<<","<<start_pos.get<2>()<<") -> ";
                // std::cout<<" to ("<<previous_pos.get<0>()<<","<<previous_pos.get<1>()<<","<<previous_pos.get<2>()<<")"<<std::endl;
                m_routing_segments.push_back(std::make_pair(start_gcell, previous_gcell));
                start_gcell = previous_gcell;
            }
            if(std::next(gcell_it) == gcells.end())
            {
                auto start_pos = m_gcell_graph->position(m_gcell_graph->graph_node(start_gcell));
                auto pos = m_gcell_graph->position(m_gcell_graph->graph_node(*gcell_it));
                // std::cout<<"Segment ("<<start_pos.get<0>()<<","<<start_pos.get<1>()<<","<<start_pos.get<2>()<<") -> ";
                // std::cout<<" to ("<<pos.get<0>()<<","<<pos.get<1>()<<","<<pos.get<2>()<<")"<<std::endl;
                m_routing_segments.push_back(std::make_pair(start_gcell, *gcell_it));
                break;
            }
            previous_gcell = *gcell_it;
            previous_direction = *direction_it;
            direction_it = std::next(direction_it);
        }
    }

    //This is required because when there is a min layer constraint
    //the pins were assigned using std::max(min_layer, pin_layer).
    void AStarRouting::connect_pins_to_min_layer()
    {
        auto& netlist = m_design.netlist();
        auto& routing_library = m_design.routing_library();
        auto& placement = m_design.placement();
        auto min_layer_index = routing_library.layerIndex(m_min_layer);
        for(auto pin : netlist.pins(m_net))
        {
            auto pin_geometry = placement.geometry(pin);
            auto pin_layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
            auto pin_layer_index = routing_library.layerIndex(pin_layer);
            if(min_layer_index > pin_layer_index)
            {
                // std::cout<<"connect pin: "<<netlist.name(pin)<<std::endl;
                auto gcell_start = m_gcell_graph->nearest_gcell(placement.location(pin), pin_layer_index-1);
                auto pos = m_gcell_graph->position(m_gcell_graph->graph_node(gcell_start));
                // std::cout<<"Start_pos ("<<pos.get<0>()<<","<<pos.get<1>()<<","<<pos.get<2>()<<")"<<std::endl;
                auto gcell_end = m_gcell_graph->nearest_gcell(placement.location(pin), min_layer_index-1);
                pos = m_gcell_graph->position(m_gcell_graph->graph_node(gcell_end));
                // std::cout<<"Start_end ("<<pos.get<0>()<<","<<pos.get<1>()<<","<<pos.get<2>()<<")"<<std::endl;
                m_routing_segments.push_back(std::make_pair(gcell_start, gcell_end));
            }
        }
    }

    void AStarRouting::write_routing_segments()
    {
        auto& global_routing = m_design.global_routing();
        auto& routing_library = m_design.routing_library();

        for(auto segment : m_routing_segments)
        {
            auto start_layer_index = m_gcell_graph->layer_index(segment.first);
            auto start_layer = routing_library.layer_from_index(start_layer_index);
            auto start_pos = m_gcell_graph->center_of_box(segment.first);

            auto end_layer_index = m_gcell_graph->layer_index(segment.second);
            auto end_layer = routing_library.layer_from_index(end_layer_index);
            auto end_pos = m_gcell_graph->center_of_box(segment.second);

            auto min_x = std::min(start_pos.x(), end_pos.x());
            auto max_x = std::max(start_pos.x(), end_pos.x());
            auto min_y = std::min(start_pos.y(), end_pos.y());
            auto max_y = std::max(start_pos.y(), end_pos.y());

            auto wire_box = box_type{{min_x, min_y}, {max_x, max_y}};
            global_routing.add_segment(wire_box, start_layer, end_layer, m_net);
        }
        global_routing.increase_demand(m_net);
    }

    void AStarRouting::clear_router_members()
    {
        m_net = net_type{};
        m_root_node = flute_node_type{};
        m_min_layer = layer_type{};
        m_routing_segments.clear();
        m_graph.clear();
    }

    //This is required because the whole routing was done assuming that all
    //pins are in same layer, but that is not true, for instance:
    //P1 (5,5,1) P2 (5,5,3) and P1 and P2 are from same net.
    //So this function seeks to connect P2 to P1 by creating a via from the
    //highest pin to lowest pin.
    void AStarRouting::connect_floating_pins()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto& routing_library = m_design.routing_library();
        std::map<point_type, std::vector<ophidian::circuit::PinInstance>, comp> pin_map;
        for(auto pin : netlist.pins(m_net))
        {
            auto pin_location = placement.location(pin);
            pin_map[pin_location].push_back(pin);
        }

        for(auto pin_vector : pin_map)
        {
            if(pin_vector.second.size() > 1)
            {
                int min_layer_index = std::numeric_limits<int>::max();
                int max_layer_index = std::numeric_limits<int>::min();
                //int max_layer_index = routing_library.layerIndex(m_min_layer);
                for(auto pin : pin_vector.second)
                {
                    auto pin_geometry = placement.geometry(pin);
                    auto pin_layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
                    auto layer_index = routing_library.layerIndex(pin_layer);
                    min_layer_index = std::min(min_layer_index, layer_index);
                    max_layer_index = std::max(max_layer_index, layer_index);
                }
                //if(min_layer_index != max_layer_index || all_pins_same_collumn)//bool used to avoid unncessary segments
                if(min_layer_index != max_layer_index)//bool used to avoid unncessary segments
                {
                    auto lower_gcell = m_gcell_graph->nearest_gcell(pin_vector.first, min_layer_index-1);
                    auto upper_gcell = m_gcell_graph->nearest_gcell(pin_vector.first, max_layer_index-1);
                    m_routing_segments.push_back(std::make_pair(lower_gcell, upper_gcell));
                }
            }
        }
    }

    void AStarRouting::trivial_routing()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto& routing_library = m_design.routing_library();

        auto pin_ref = *netlist.pins(m_net).begin();
        auto ref_loc = placement.location(pin_ref);

        int min_layer_index = std::numeric_limits<int>::max();
        int max_layer_index = std::numeric_limits<int>::min();
        for(auto pin : netlist.pins(m_net))
        {
            auto pin_geometry = placement.geometry(pin);
            auto pin_layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
            auto layer_index = routing_library.layerIndex(pin_layer);
            min_layer_index = std::min(min_layer_index, layer_index);
            max_layer_index = std::max(max_layer_index, layer_index);
        }
        auto lower_gcell = m_gcell_graph->nearest_gcell(ref_loc, min_layer_index-1);
        auto upper_gcell = m_gcell_graph->nearest_gcell(ref_loc, max_layer_index-1);
        m_routing_segments.push_back(std::make_pair(lower_gcell, upper_gcell));
    }

    bool AStarRouting::all_pins_same_collumn()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto pin_ref = *netlist.pins(m_net).begin();
        auto ref_loc = placement.location(pin_ref);
        for(auto pin : netlist.pins(m_net))
        {
            auto pin_loc = placement.location(pin);
            if(ref_loc.x() != pin_loc.x() || ref_loc.y() != pin_loc.y())
                return false;
        }
        //std::cout<<"same gcell net: "<<netlist.name(m_net)<<std::endl;
        return true;
    }

    void AStarRouting::update_extra_demand_constraint()
    {
        auto& netlist = m_design.netlist();
        auto& global_routing = m_design.global_routing();
        auto gcell_graph = global_routing.gcell_graph();
        auto & routing_library = m_design.routing_library();

        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;

        std::unordered_map<circuit::StandardCells::cell_type, std::unordered_set<routing::GCellGraph::gcell_type, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> gcells_per_std_cell;

        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++)
        {
            auto cell = *cell_it;
            auto location = m_design.placement().location(cell);
            auto std_cell = m_design.netlist().std_cell(cell);

            auto cell_name = m_design.netlist().name(cell);
            auto std_cell_name = m_design.standard_cells().name(std_cell);

            // for (auto layer_it = routing_library.begin_layer(); layer_it != routing_library.end_layer(); layer_it++) {
            // auto layer = *layer_it;
            // auto layer_index = routing_library.layerIndex(layer);
            // auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);
            for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++)
            {
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);

                //std::cout << "cell " << cell_name << " std cell " << std_cell_name << " location " << location.x().value() << "," << location.y().value() << "," << layer_index;
                //auto gcell_box = gcell_graph->box(gcell);
                //std::cout << " gcell " << gcell_box.min_corner().x().value() << "," << gcell_box.min_corner().y().value() << std::endl;

                std_cells_per_gcell[gcell][std_cell] += 1;
                gcells_per_std_cell[std_cell].insert(gcell);
            }
        }

        auto & routing_constraints = m_design.routing_constraints();
        for (auto same_grid_it = routing_constraints.begin_same_grid(); same_grid_it != routing_constraints.end_same_grid(); same_grid_it++)
        {
            auto key = same_grid_it->first;
            auto & extra_demand = same_grid_it->second;
            auto demand = extra_demand.demand;

            auto macro1_name = extra_demand.macro1;
            auto macro2_name = extra_demand.macro2;
            auto layer_name = extra_demand.layer;

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);

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
                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);

                if (layer == gcell_layer)
                {
                    auto pair_count = std::min(std_cells_per_gcell[gcell][macro1], std_cells_per_gcell[gcell][macro2]);
                    m_gcells_extra_demand[gcell] += pair_count * demand;
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

            auto macro1 = m_design.standard_cells().find_cell(macro1_name);
            auto macro2 = m_design.standard_cells().find_cell(macro2_name);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);

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
                auto gcell_node = gcell_graph->graph_node(gcell);
                auto east_node = gcell_graph->east_node(gcell_node);
                auto east_gcell = gcell_graph->gcell(east_node);
                auto west_node = gcell_graph->west_node(gcell_node);
                auto west_gcell = gcell_graph->gcell(west_node);

                auto gcell_layer_index = gcell_graph->layer_index(gcell);
                auto gcell_layer_name = "M" + boost::lexical_cast<std::string>(gcell_layer_index);
                auto gcell_layer = m_design.routing_library().find_layer_instance(gcell_layer_name);
                if (layer == gcell_layer)
                {
                    auto east_pair_count = 0;
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
                    m_gcells_extra_demand[gcell] += (east_pair_count + west_pair_count) * demand;
                }
            }
        }
    }


    bool AStarRouting::gcell_has_free_space(AStarRouting::gcell_type gcell)
    {
        return ((m_gcell_graph->capacity(gcell) - 1) >= (m_gcell_graph->demand(gcell) + m_gcells_extra_demand[gcell]));
    }
}
