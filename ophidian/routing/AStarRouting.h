#ifndef OPHIDIAN_ASTARROUTING_H
#define OPHIDIAN_ASTARROUTING_H

#include <ophidian/design/Design.h>
#include <ophidian/entity_system/Property.h>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/routing/Library.h>
#include <lemon/concepts/graph.h>
#include <memory>
#include <limits>

//TODO:
//1-convert vector of gcells into GlobalRouting segments
//2-consider extra demand constraints
namespace ophidian::routing
{

    struct FluteNode;
    struct AStarNode
    {
        using gcell_type = ophidian::routing::GCell;

        AStarNode():g{std::numeric_limits<unsigned int>::max()},h{0},finished{false},discovered{false},gcell_from{}{};

        unsigned int g, h;//g is the cost from start up to this node, h is heuristic value
        gcell_type gcell_from;//useful for backtracking result
        bool finished, discovered;
    };

    class AStarRouting
    {
        public:
            using net_type                          = circuit::Net;
            using design_type                       = design::Design;
            using pin_name_type                     = std::string;
            using point_type                        = util::LocationDbu;
            using gcell_type                        = GCell;
            using gcell_property_type               = entity_system::Property<gcell_type, AStarNode>;
            using gcell_graph_ptr_type              = std::shared_ptr<ophidian::routing::GCellGraph>;
            using layer_type                        = Layer;
            using flute_graph_type                  = lemon::ListGraph;
            using flute_node_type                   = lemon::ListGraph::Node;
            using node_map_type                     = flute_graph_type::NodeMap<FluteNode>;
            using edge_map_type                     = flute_graph_type::EdgeMap<std::vector<gcell_type>>;

            AStarRouting(design_type & design);

            bool route_net(const net_type & net);
        private:
            void init_flute_graph(const net_type & net);
            void map_flute_nodes_into_gcells();
            void route_flute_segments();
            void a_star(flute_node_type start, flute_node_type goal);
            unsigned int heuristic(const gcell_type & source, const gcell_type & goal, bool goal_is_steiner) const;
            std::vector<gcell_type> neighbors(gcell_type gcell);
            void update_f_score(gcell_type gcell, gcell_type neighbor_gcell, gcell_type goal_gcell, bool goal_is_steiner);
            bool goal_reached(const gcell_type & source, const gcell_type & goal, bool goal_is_steiner) const;
            void print_routing(flute_node_type root_node);
            void back_track_path(flute_node_type s, flute_node_type g);
            void print_path(flute_node_type s, flute_node_type g);

            flute_graph_type                      m_graph;
            node_map_type                         m_node_map;
            edge_map_type                         m_edge_map;
            gcell_property_type                   m_gcell_to_AStarNode;
            design_type&                          m_design;
            gcell_graph_ptr_type                  m_gcell_graph;
            layer_type                            m_min_layer;
    };

    struct FluteNode
    {
        using point_type    = AStarRouting::point_type;
        using pin_name_type = AStarRouting::pin_name_type;
        using gcell_type    = AStarRouting::gcell_type;

        FluteNode() = default;//required by lemon::NodeMap

        FluteNode(point_type p, pin_name_type name) : point{p}, pin_name{name}{}

        point_type        point;
        pin_name_type     pin_name;
        gcell_type        mapped_gcell;

        friend std::ostream& operator<<(std::ostream& os, const FluteNode& pin)
        {
            os <<"("<<pin.point.x().value()<<", "<<pin.point.y().value()<<", "<<pin.pin_name<<") ";
            return os;
        }
    };
}

#endif // OPHIDIAN_ASTARROUTING_H
