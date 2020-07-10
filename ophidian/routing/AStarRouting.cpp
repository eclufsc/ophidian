#include <ophidian/routing/AStarRouting.h>
#include <queue>
#include <list>

bool AStarDebug = false;

namespace ophidian::routing
{

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
        m_gcell_to_AStarNode{design.global_routing().gcell_graph()->make_property_gcells(AStarNode{})}
    {
    }

    bool AStarRouting::route_net(const AStarRouting::net_type & net)
    {
        //would be nice, first check for existent routing and undo that routing.
        auto& netlist = m_design.netlist();
        auto& routing_constraints = m_design.routing_constraints();
        auto net_size = netlist.pins(net).size();
        m_min_layer = routing_constraints.min_net_layer(net);

        //TODO not only consider net pin size but also number of different gcells.
        //TODO don't forget min layer_constraint.
        if(net_size > 2)
        {
            //1-Call Flute.
            init_flute_graph(net);
            //2-Connect each pair of nodes using A*.(consider min layer)
            route_flute_segments();
        }else if( net_size == 2)
        {
            // find_path(nodes, s ,t);
            //perform the Astar receiving only the graph and 2 gcells.
            //consider min layer constr.
        }else
        {
            /*
            // For iccad2020 it should never happen.
            //LOCAL NET: only one node
            auto s = *netlist.pins(*net_it).begin();
            nodes.insert(node_of_pin(s));
            */
        }
        //expand window
        return true;
    }

    void AStarRouting::init_flute_graph(const AStarRouting::net_type & net)
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        std::map<point_type, std::string, comp> pins_map;
        std::map<point_type, flute_node_type, comp> created_nodes;

        //Run Flute
        std::vector<interconnection::Flute::Point> net_points;
        auto net_pins = netlist.pins(net);
        net_points.reserve(netlist.pins(net).size());
        for(auto pin : net_pins)
        {
            auto pin_location = placement.location(pin);
            net_points.push_back(pin_location);
            auto pin_name = netlist.name(pin);
            pins_map[pin_location] = pin_name;
        }
        auto & flute = interconnection::Flute::instance();
        auto tree = flute.create(net_points);

        //Create FluteGraph
        for(auto tree_segment_it = tree->segments().first; tree_segment_it != tree->segments().second; ++tree_segment_it)
        {
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

        //sort of layer assignment
        map_flute_nodes_into_gcells();
    }

    //This mapping is considering net min. layer constraint and gcell capacity
    void AStarRouting::map_flute_nodes_into_gcells()
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
                layer_index = std::max(layer_index, pin_layer_index);
            }
            auto gcell = m_gcell_graph->nearest_gcell(flute_node.point, layer_index-1);

            // auto s_node = m_gcell_graph->graph_node(gcell);
            // auto s_pos = m_gcell_graph->position(s_node);
            // std::cout<<"mapping node: "<<flute_node<<" into "<<" pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;

            if(m_gcell_graph->has_free_space(gcell))
                flute_node.mapped_gcell = gcell;
            else
                std::cout<<"WARNING: mapped node is overflowed!!!"<<std::endl;//TODO: handle this checking the up node
            /*
            auto pos = m_gcell_graph->position(m_gcell_graph->graph_node(flute_node.mapped_gcell));
            std::cout<<flute_node<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;
            */

            auto s_node = m_gcell_graph->graph_node(flute_node.mapped_gcell);
            auto s_pos = m_gcell_graph->position(s_node);
            std::cout<<" again mapping node: "<<flute_node<<" into "<<" pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;
        }
    }

    //call a_star routing for each pair of flute nodes in BFS order from any node_pin
    void AStarRouting::route_flute_segments()
    {
        flute_node_type root_node;
        flute_graph_type::NodeMap<bool> visited_nodes{m_graph};
        for(flute_graph_type::NodeIt node(m_graph); node != lemon::INVALID; ++node)
        {
            if(m_node_map[node].pin_name != "steiner")
            {
                root_node = node;
                break;
            }
        }
        std::cout<<"root node: "<<m_node_map[root_node]<<std::endl;
        std::queue<flute_node_type> queue;
        queue.push(root_node);

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
        //TODO:if min layer create towers(connect pins to min layers)
        print_routing(root_node);
    }

    //find a shortest path from start.mapped_gcell to goal.mapped_gcell
    void AStarRouting::a_star(flute_node_type start, flute_node_type goal)
    {
        bool goal_is_steiner = m_node_map[goal].pin_name == "steiner" ? true : false;
        std::list<gcell_type> open_nodes;
        std::vector<gcell_type> dirty_nodes;
        auto node = m_gcell_graph->graph_node(m_node_map[start].mapped_gcell);
        auto pos = m_gcell_graph->position(node);
        std::cout<<m_node_map[start]<<std::endl;
        std::cout<<"start node: "<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;
        node = m_gcell_graph->graph_node(m_node_map[goal].mapped_gcell);
        pos = m_gcell_graph->position(node);
        std::cout<<m_node_map[goal]<<std::endl;
        std::cout<<"goal node: "<<pos.get<0>()<<", "<<pos.get<1>()<<", "<<pos.get<2>()<<std::endl;

        open_nodes.push_back(m_node_map[start].mapped_gcell);

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
        if(open_nodes.empty())
            std::cout<<"there is no path to the goal"<<std::endl;//TODO: handle this with a boolean return
        if(goal_is_steiner)
            m_node_map[goal].mapped_gcell = current_node;

        std::cout<<"print result"<<std::endl;
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
            if(m_gcell_graph->has_free_space(neighbor_gcell))
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
            if(m_gcell_graph->has_free_space(neighbor_gcell))
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
                if(m_gcell_graph->has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
            auto east_node = m_gcell_graph->east_node(node);
            if(east_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(east_node);
                if(m_gcell_graph->has_free_space(neighbor_gcell))
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
                if(m_gcell_graph->has_free_space(neighbor_gcell))
                    if(m_gcell_to_AStarNode[neighbor_gcell].finished == false)
                        result.push_back(neighbor_gcell);
            }
            auto south_node = m_gcell_graph->south_node(node);
            if(south_node != lemon::INVALID)//check for graph boundary
            {
                auto neighbor_gcell = m_gcell_graph->gcell(south_node);
                if(m_gcell_graph->has_free_space(neighbor_gcell))
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
    void AStarRouting::print_routing(AStarRouting::flute_node_type root_node)
    {
        flute_graph_type::NodeMap<bool> visited_nodes{m_graph};
        std::queue<flute_node_type> queue;
        queue.push(root_node);

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
                    print_path(current_flute_node, opposite_flute_node);
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

        std::cout<<"backtrack from (G) "<<m_node_map[g]<<" to (S) "<<m_node_map[s]<<std::endl;
        std::cout<<"S pos: ("<<s_pos.get<0>()<<","<<s_pos.get<1>()<<","<<s_pos.get<2>()<<")"<<std::endl;
        std::cout<<"G pos: ("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;

        bool non_stop = true;
        while(non_stop)
        {
            non_stop = (s_pos.get<0>() != g_pos.get<0>() || s_pos.get<1>() != g_pos.get<1>() || s_pos.get<2>() != g_pos.get<2>());
            std::cout<<"("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;
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

    void AStarRouting::print_path(AStarRouting::flute_node_type s, AStarRouting::flute_node_type g)
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
        std::cout<<"RESULT from (G) "<<m_node_map[g]<<" to (S) "<<m_node_map[s]<<std::endl;
        for(auto gcell : gcells)
        {
            auto g_node = m_gcell_graph->graph_node(gcell);
            auto g_pos = m_gcell_graph->position(g_node);
            std::cout<<"("<<g_pos.get<0>()<<","<<g_pos.get<1>()<<","<<g_pos.get<2>()<<")"<<std::endl;
        }
    }
}
