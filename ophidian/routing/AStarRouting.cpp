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
        m_same_gcell_extra_demand{design.global_routing().gcell_graph()->make_property_gcells(int{0})},
        m_adj_gcell_extra_demand{design.global_routing().gcell_graph()->make_property_gcells(int{0})},
        m_gcells_cell_instances{design.global_routing().gcell_graph()->make_property_gcells<cell_set_type>()}
    {
        auto& placement = design.placement();
        auto& netlist = design.netlist();
        auto gcell_graph_ptr = design.global_routing().gcell_graph();
        for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); cell_it++)
        {
            auto cell = *cell_it;
            auto location = placement.location(cell);
            auto gcell = gcell_graph_ptr->nearest_gcell(location, 0);
            m_gcells_cell_instances[gcell].insert(cell);
        }
        update_extra_demand_constraint();
    }

    //when routing all nets is required first remove all of them
    bool AStarRouting::route_net(const AStarRouting::net_type & net, std::vector<AStarSegment> & segments, bool applying_routing)
    {
        m_net = net;
        auto& netlist = m_design.netlist();
        auto& routing_constraints = m_design.routing_constraints();
        auto net_size = netlist.pins(net).size();
        m_min_layer = routing_constraints.min_net_layer(net);

        bool all_pins_are_in_same_collumn = all_pins_same_collumn();
        if(all_pins_are_in_same_collumn == false)
        {
            bool flute_graph;
            if(net_size > 2)
                flute_graph = init_flute_graph();
            else if(net_size == 2)
                flute_graph = init_two_pin_flute_graph();
            else//net only have 1 pin (For iccad2020 it should never happen)
                return false;

            if(flute_graph == false)
                return false;
            bool working_correct = node_layer_assignment();
            if(working_correct == false)
            {
                clear_router_members();
                return false;
            }
            working_correct = route_flute_segments();
            if(working_correct == false)
            {
                clear_router_members();
                return false;
            }
            bfs_backtrack();
            working_correct = connect_pins_to_min_layer();
            if(working_correct == false)
            {
                clear_router_members();
                return false;
            }
            working_correct = connect_floating_pins();
            if(working_correct == false)
            {
                clear_router_members();
                return false;
            }
        }
        else
        {
            bool working_correct = trivial_routing();
            if(working_correct == false)
            {
                clear_router_members();
                return false;
            }
        }
        generate_routing_segments(segments);
        bool solution_has_no_overflow = true;
        if(applying_routing)
            solution_has_no_overflow = apply_segments_to_global_routing(segments);
        clear_router_members();
        if(solution_has_no_overflow == false)
            return false;
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
        net_points.reserve(netlist.pins(m_net).size());
        for(auto pin : net_pins)
        {
            auto pin_location = placement.location(pin);
            net_points.push_back(pin_location);
            auto pin_name = netlist.name(pin);
            pins_map[pin_location] = pin_name;
        }
        auto & flute = interconnection::Flute::instance();
        auto tree = flute.create(net_points);

        auto steiner_tree_length = tree->length().value();
        if(steiner_tree_length == 0)
        {
            if (AStarDebug) std::cout<<"WARNING: same gcell"<<std::endl;
            if (AStarDebug) std::cout<<"BUG in function: bool all_pins_same_collumn()"<<std::endl;
            return false;
        }

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
            if (AStarDebug) std::cout<<"WARNING: same gcell"<<std::endl;
            if (AStarDebug) std::cout<<"BUG in function: bool all_pins_same_collumn()"<<std::endl;
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
    bool AStarRouting::node_layer_assignment()
    {
        auto& routing_library = m_design.routing_library();
        auto& placement = m_design.placement();
        auto& netlist = m_design.netlist();
        for(flute_graph_type::NodeIt node(m_graph); node != lemon::INVALID; ++node)
        {
            auto& flute_node = m_node_map[node];
            int layer_index = routing_library.layerIndex(m_min_layer);
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
            flute_node.mapped_gcell = gcell;

            //Because for steiner nodes the Z does not matter and the A* will find a free node in that collumn
            //This check just make sense when moving cells:
            if(flute_node.pin_name != "steiner" && gcell_has_free_space(gcell) == false)
            {
                if (AStarDebug) std::cout<<"WARNING: pin node mapped is overflowed, please undo that cell movement!!!"<<std::endl;
                return false;
            }
        }
        return true;
    }

    //call a_star routing for each pair of flute nodes in BFS order from any node_pin
    bool AStarRouting::route_flute_segments()
    {
        flute_graph_type::NodeMap<bool> visited_nodes{m_graph};
        for(flute_graph_type::NodeIt node(m_graph); node != lemon::INVALID; ++node)
        {
            if(m_node_map[node].pin_name != "steiner")
            {
                m_root_node = node;
                break;
            }
        }
        std::queue<flute_node_type> queue;
        queue.push(m_root_node);

        while(!queue.empty())
        {
            auto current_node = queue.front();
            queue.pop();
            visited_nodes[current_node] = true;
            for(flute_graph_type::IncEdgeIt edge(m_graph, current_node); edge != lemon::INVALID; ++edge)
            {
                auto opposite_node = m_graph.oppositeNode(current_node, edge);
                if(visited_nodes[opposite_node] == false)
                {
                    queue.push(opposite_node);
                    bool exist_path = a_star(current_node, opposite_node);
                    if(exist_path == false)
                        return false;
                }
            }
        }
        return true;
    }

    //find a shortest path from start.mapped_gcell to goal.mapped_gcell
    bool AStarRouting::a_star(flute_node_type start, flute_node_type goal)
    {
        bool goal_is_steiner = m_node_map[goal].pin_name == "steiner" ? true : false;
        std::list<gcell_type> open_nodes;
        open_nodes.push_back(m_node_map[start].mapped_gcell);
        std::vector<gcell_type> dirty_nodes;
        dirty_nodes.push_back(m_node_map[start].mapped_gcell);
        m_gcell_to_AStarNode[m_node_map[start].mapped_gcell].g = 0;

        gcell_type current_node;
        bool target_found = false;
        while(open_nodes.empty() == false)
        {
            current_node = open_nodes.front();
            if(goal_reached(current_node, m_node_map[goal].mapped_gcell, goal_is_steiner))
            {
                target_found = true;
                break;
            }
            open_nodes.pop_front();

            //discover neighbors
            auto current_neighbors = neighbors(current_node);
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

        //IMPORTANT: return false (return AFTER clean dirty nodes)
        if(open_nodes.empty())
            if (AStarDebug) std::cout<<"there is no path to the goal"<<std::endl;
        if(goal_is_steiner)
            m_node_map[goal].mapped_gcell = current_node;
        if(target_found)
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
        return target_found;
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

        bool non_stop = true;
        while(non_stop)
        {
            non_stop = (s_pos.get<0>() != g_pos.get<0>() || s_pos.get<1>() != g_pos.get<1>() || s_pos.get<2>() != g_pos.get<2>());
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
                m_routing_segments.push_back(std::make_pair(start_gcell, previous_gcell));
                start_gcell = previous_gcell;
            }
            if(std::next(gcell_it) == gcells.end())
            {
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
    bool AStarRouting::connect_pins_to_min_layer()
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
                for(auto layer_index = pin_layer_index; layer_index != min_layer_index; layer_index++)
                {
                    auto gcell = m_gcell_graph->nearest_gcell(placement.location(pin), layer_index-1);
                    if(gcell_has_free_space(gcell) == false)
                        return false;
                }
                auto gcell_start = m_gcell_graph->nearest_gcell(placement.location(pin), pin_layer_index-1);
                auto gcell_end = m_gcell_graph->nearest_gcell(placement.location(pin), min_layer_index-1);
                m_routing_segments.push_back(std::make_pair(gcell_start, gcell_end));
            }
        }
        return true;
    }

    void AStarRouting::generate_routing_segments(std::vector<AStarSegment> & segments)
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

            segments.push_back(AStarSegment(wire_box, start_layer, end_layer, m_net));
        }
    }

    //Be carefull if you call this twice for the same net it will mess up the demand!
    bool AStarRouting::apply_segments_to_global_routing(const std::vector<AStarSegment> & segments)
    {
        auto& global_routing = m_design.global_routing();
        std::unordered_set<net_type, entity_system::EntityBaseHash> nets;
        for(auto segment : segments)
        {
            global_routing.add_segment(segment.wire_box, segment.start_layer, segment.end_layer, segment.net);
            nets.insert(segment.net);
        }
        //IMPORANT: these two following loops CAN NOT be merged.
        for(auto net: nets)
            global_routing.increase_demand(net);
        for(auto net: nets)
            for(auto gcell : global_routing.gcells(net))
                if(m_gcell_graph->capacity(gcell) < (m_gcell_graph->demand(gcell) + m_same_gcell_extra_demand[gcell] + m_adj_gcell_extra_demand[gcell]))
                    return false;
        return true;
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
    bool AStarRouting::connect_floating_pins()
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
            if(pin_vector.second.size() > 1)//more than one pin in same collumn
            {
                int min_layer_index = std::numeric_limits<int>::max();
                int max_layer_index = routing_library.layerIndex(m_min_layer);
                for(auto pin : pin_vector.second)
                {
                    auto pin_geometry = placement.geometry(pin);
                    auto pin_layer_name = pin_geometry.front().second;
                    auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
                    auto layer_index = routing_library.layerIndex(pin_layer);
                    min_layer_index = std::min(min_layer_index, layer_index);
                    max_layer_index = std::max(max_layer_index, layer_index);
                }
                if(min_layer_index != max_layer_index)
                {
                    for(auto layer_index = min_layer_index; layer_index != max_layer_index; layer_index++)
                    {
                        auto gcell = m_gcell_graph->nearest_gcell(pin_vector.first, layer_index-1);
                        if(gcell_has_free_space(gcell) == false)
                            return false;
                    }
                    auto lower_gcell = m_gcell_graph->nearest_gcell(pin_vector.first, min_layer_index-1);
                    auto upper_gcell = m_gcell_graph->nearest_gcell(pin_vector.first, max_layer_index-1);
                    m_routing_segments.push_back(std::make_pair(lower_gcell, upper_gcell));
                }
            }
        }
        return true;
    }

    bool AStarRouting::trivial_routing()
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        auto& routing_library = m_design.routing_library();

        auto pin_ref = *netlist.pins(m_net).begin();
        auto ref_loc = placement.location(pin_ref);

        int min_layer_index = std::numeric_limits<int>::max();
        int max_layer_index = routing_library.layerIndex(m_min_layer);
        for(auto pin : netlist.pins(m_net))
        {
            auto pin_geometry = placement.geometry(pin);
            auto pin_layer_name = pin_geometry.front().second;
            auto pin_layer = routing_library.find_layer_instance(pin_layer_name);
            auto layer_index = routing_library.layerIndex(pin_layer);
            min_layer_index = std::min(min_layer_index, layer_index);
            max_layer_index = std::max(max_layer_index, layer_index);
        }
        for(auto layer_index = min_layer_index; layer_index != max_layer_index; layer_index++)
        {
            auto gcell = m_gcell_graph->nearest_gcell(ref_loc, layer_index-1);
            if(gcell_has_free_space(gcell) == false)
                return false;
        }
        auto lower_gcell = m_gcell_graph->nearest_gcell(ref_loc, min_layer_index-1);
        auto upper_gcell = m_gcell_graph->nearest_gcell(ref_loc, max_layer_index-1);
        m_routing_segments.push_back(std::make_pair(lower_gcell, upper_gcell));
        return true;
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
        return true;
    }

    void AStarRouting::move_cell(AStarRouting::cell_instance_type cell, AStarRouting::gcell_type source_gcell, AStarRouting::gcell_type target_gcell)
    {
        m_gcells_cell_instances[source_gcell].erase(cell);
        m_gcells_cell_instances[target_gcell].insert(cell);
    }

    //Only receive GCells from layer 0 as parameter!
    void AStarRouting::update_extra_demand_constraint(AStarRouting::gcell_type gcell)
    {
        auto & global_routing = m_design.global_routing();
        auto gcell_graph_ptr = global_routing.gcell_graph();

        update_same_extra_demand(gcell);
        update_adj_extra_demand(gcell);

        //Update potential affected gcells adj extra demand
        auto node = gcell_graph_ptr->graph_node(gcell);
        auto east_node = gcell_graph_ptr->east_node(node);
        if(east_node != lemon::INVALID)
            update_adj_extra_demand(gcell_graph_ptr->gcell(east_node));
        auto west_node = gcell_graph_ptr->west_node(node);
        if(west_node != lemon::INVALID)
            update_adj_extra_demand(gcell_graph_ptr->gcell(west_node));
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

            for (auto layer_index = routing_library.lowest_layer_index(); layer_index <= routing_library.highest_layer_index(); layer_index++)
            {
                auto gcell = gcell_graph->nearest_gcell(location, layer_index-1);

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
                    m_same_gcell_extra_demand[gcell] += pair_count * demand;
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
                    m_adj_gcell_extra_demand[gcell] += (east_pair_count + west_pair_count) * demand;
                }
            }
        }
    }

    bool AStarRouting::gcell_has_free_space(AStarRouting::gcell_type gcell)
    {
        return ((m_gcell_graph->capacity(gcell) - 1) >= (m_gcell_graph->demand(gcell) + m_same_gcell_extra_demand[gcell] + m_adj_gcell_extra_demand[gcell]));
    }

    void AStarRouting::update_same_extra_demand(const AStarRouting::gcell_type & gcell)
    {
        auto & netlist = m_design.netlist();
        auto & routing_library = m_design.routing_library();
        auto & standard_cells = m_design.standard_cells();
        auto & routing_constraints = m_design.routing_constraints();
        auto & global_routing = m_design.global_routing();
        auto gcell_graph_ptr = global_routing.gcell_graph();

        //Clear current same extra demand
        auto node = gcell_graph_ptr->graph_node(gcell);
        auto gcell_position = gcell_graph_ptr->position(node);
        for(auto layer_it = routing_library.begin_layer(); layer_it != routing_library.end_layer(); layer_it++)
        {
            auto layer_index = routing_library.layerIndex(*layer_it);
            auto affected_node = gcell_graph_ptr->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = gcell_graph_ptr->gcell(affected_node);
            m_same_gcell_extra_demand[affected_gcell] = 0;
        }

        //Count macros from gcells
        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        for(auto cell_it = m_gcells_cell_instances[gcell].begin(); cell_it != m_gcells_cell_instances[gcell].end(); cell_it++)
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
            auto layer = routing_library.find_layer_instance(layer_name);
            auto layer_index = routing_library.layerIndex(layer);
            auto macro1 = standard_cells.find_cell(extra_demand.macro1);
            auto macro2 = standard_cells.find_cell(extra_demand.macro2);
            int macro1_count = std_cells_per_gcell[gcell].find(macro1) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro1];
            int macro2_count = std_cells_per_gcell[gcell].find(macro2) == std_cells_per_gcell[gcell].end() ? 0 : std_cells_per_gcell[gcell][macro2];
            int pair_count = std::min(macro1_count, macro2_count);
            auto affected_node = gcell_graph_ptr->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = gcell_graph_ptr->gcell(affected_node);
            m_same_gcell_extra_demand[affected_gcell] += pair_count * demand;
        }
    }

    void AStarRouting::update_adj_extra_demand(const AStarRouting::gcell_type & gcell)
    {
        auto & netlist = m_design.netlist();
        auto & routing_library = m_design.routing_library();
        auto & standard_cells = m_design.standard_cells();
        auto & routing_constraints = m_design.routing_constraints();
        auto & global_routing = m_design.global_routing();
        auto gcell_graph_ptr = global_routing.gcell_graph();

        //Clear current adj extra demand
        auto node = gcell_graph_ptr->graph_node(gcell);
        auto gcell_position = gcell_graph_ptr->position(node);
        for(auto layer_it = routing_library.begin_layer(); layer_it != routing_library.end_layer(); layer_it++)
        {
            auto layer_index = routing_library.layerIndex(*layer_it);
            auto affected_node = gcell_graph_ptr->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = gcell_graph_ptr->gcell(affected_node);
            m_adj_gcell_extra_demand[affected_gcell] = 0;
        }

        //Count macros from adj gcells
        std::unordered_map<routing::GCellGraph::gcell_type, std::unordered_map<circuit::StandardCells::cell_type, int, entity_system::EntityBaseHash>, entity_system::EntityBaseHash> std_cells_per_gcell;
        gcell_set_type gcells_to_count{gcell};
        auto east_node = gcell_graph_ptr->east_node(node);
        auto east_gcell = gcell_graph_ptr->gcell(east_node);
        if(east_node != lemon::INVALID)
            gcells_to_count.insert(gcell_graph_ptr->gcell(east_node));
        auto west_node = gcell_graph_ptr->west_node(node);
        auto west_gcell = gcell_graph_ptr->gcell(west_node);
        if(west_node != lemon::INVALID)
            gcells_to_count.insert(gcell_graph_ptr->gcell(west_node));
        for(auto gcell : gcells_to_count)
        {
            for(auto cell_it = m_gcells_cell_instances[gcell].begin(); cell_it != m_gcells_cell_instances[gcell].end(); cell_it++)
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
            auto macro1 = standard_cells.find_cell(extra_demand.macro1);
            auto macro2 = standard_cells.find_cell(extra_demand.macro2);
            auto layer = m_design.routing_library().find_layer_instance(layer_name);
            auto layer_index = routing_library.layerIndex(layer);

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
            auto affected_node = gcell_graph_ptr->node(gcell_position.get<0>(), gcell_position.get<1>(), layer_index-1);
            auto affected_gcell = gcell_graph_ptr->gcell(affected_node);
            m_adj_gcell_extra_demand[affected_gcell] += (east_pair_count + west_pair_count) * demand;
        }

    }
}
