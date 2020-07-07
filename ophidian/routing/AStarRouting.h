#ifndef OPHIDIAN_ASTARROUTING_H
#define OPHIDIAN_ASTARROUTING_H

#include <ophidian/design/Design.h>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/interconnection/Flute.h>
#include <lemon/concepts/graph.h>

//0 don't consider any constraint.
//1 consider routing layer directions.
//2 consider cap and demand.
//3 consider extra demand constraint.
//4 consider min layer constr.
//5 consider only relevant nodes.

namespace ophidian::routing
{

    struct FluteNode;

    class AStarRouting
    {
        public:
            using net_type                          = circuit::Net;
            using design_type                       = design::Design;
            using pin_name_type                     = std::string;
            using point_type                        = util::LocationDbu;
            using flute_graph_type                  = lemon::ListGraph;
            using node_type                         = lemon::ListGraph::Node;
            using node_map_type                     = flute_graph_type::NodeMap<FluteNode>;

            AStarRouting(design_type & design) : m_graph{}, m_design{design}, m_node_map{m_graph}{}

            bool route_net(const net_type & net);
        private:
            void init_flute_graph(const net_type & net);

            flute_graph_type m_graph;
            node_map_type m_node_map;
            design_type& m_design;
    };

    struct FluteNode{
        using point_type = AStarRouting::point_type;
        using pin_name_type = AStarRouting::pin_name_type;

        FluteNode() = default;//required by lemon::NodeMap

        FluteNode(point_type p, pin_name_type name) : point{p}, pin_name{name}{}

        point_type point;
        pin_name_type pin_name;

        friend std::ostream& operator<<(std::ostream& os, const FluteNode& pin)
        {
            os <<"("<<pin.point.x().value()<<", "<<pin.point.y().value()<<", "<<pin.pin_name<<") ";
            return os;
        }
    };

}

#endif // OPHIDIAN_ASTARROUTING_H
