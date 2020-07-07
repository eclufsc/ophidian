#include <ophidian/routing/AStarRouting.h>

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

    void AStarRouting::init_flute_graph(const AStarRouting::net_type & net)
    {
        auto& netlist = m_design.netlist();
        auto& placement = m_design.placement();
        std::map<point_type, std::string, comp> pins_map;
        std::map<point_type, node_type, comp> created_nodes;

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

        for(auto tree_segment_it = tree->segments().first; tree_segment_it != tree->segments().second; ++tree_segment_it)
        {
            auto tree_segment = *tree_segment_it;
            auto segment_start = tree->position(tree->u(tree_segment));
            auto segment_end = tree->position(tree->v(tree_segment));

            if (AStarDebug) std::cout << "tree segment " << segment_start.x().value() << "," << segment_start.y().value() << "->" << segment_end.x().value() << "," << segment_end.y().value() << std::endl;

            auto pin1_name = pins_map.find(segment_start) != pins_map.end() ? pins_map[segment_start] : "steiner";
            node_type node1;
            if(created_nodes.count(segment_start))
                node1 = created_nodes[segment_start];
            else
            {
                node1 = m_graph.addNode();
                created_nodes[segment_start] = node1;
                m_node_map[node1] = FluteNode{segment_start, pin1_name};
            }

            auto pin2_name = pins_map.find(segment_end) != pins_map.end() ? pins_map[segment_end] : "steiner";
            node_type node2;
            if(created_nodes.count(segment_end))
                node2 = created_nodes[segment_end];
            else
            {
                node2 = m_graph.addNode();
                created_nodes[segment_end] = node2;
                m_node_map[node2] = FluteNode{segment_end, pin2_name};
            }
            m_graph.addEdge(node1, node2);
        }
    }

    bool AStarRouting::route_net(const AStarRouting::net_type & net)
    {
        auto& netlist = m_design.netlist();
        auto net_size = netlist.pins(net).size();

        //TODO not only consider net pin size but also number of different gcells.
        //TODO don't forget min layer_constraint.
        if(net_size > 2)
        {
            //1-Call Flute.
            init_flute_graph(net);
            for (lemon::ListGraph::NodeIt n(m_graph); n != lemon::INVALID; ++n)
            {
                std::cout<<m_node_map[n]<<std::endl;
            }
            // for(auto seg : segments_flute)
            //     std::cout<<seg<<std::endl;
            //4-Connect each pair of nodes using A*.(consider min layer)
            //TODO
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
}
