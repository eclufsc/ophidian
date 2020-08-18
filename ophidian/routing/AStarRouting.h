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
#include <unordered_set>
#include <limits>

#include <ophidian/util/log.h>

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

    struct AStarSegment
    {
        using unit_type                         = util::database_unit_t;
        using box_type                          = geometry::Box<unit_type>;
        using layer_type                        = Layer;
        using net_type                          = circuit::Net;
        
        AStarSegment():
            wire_box{box_type{{unit_type{0}, unit_type{0}}, {unit_type{0}, unit_type{0}}}},
            start_layer{layer_type{}},
            end_layer{layer_type{}},
            net{net_type{}}
        {};

        AStarSegment(box_type box, layer_type start, layer_type end, net_type n):
            wire_box{box},
            start_layer{start},
            end_layer{end},
            net{n}
        {};

        box_type wire_box;
        layer_type start_layer, end_layer;
        net_type net;
    };

    class AStarRouting
    {
        public:
            using net_type                          = circuit::Net;
            using cell_instance_type                = circuit::CellInstance;
            using unit_type                         = util::database_unit_t;
            using design_type                       = design::Design;
            using pin_name_type                     = std::string;
            using point_type                        = util::LocationDbu;
            using box_type                          = geometry::Box<unit_type>;
            using gcell_type                        = GCell;
            using gcell_property_type               = entity_system::Property<gcell_type, AStarNode>;
            using gcell_extra_demand_type           = entity_system::Property<gcell_type, int>;
            using cell_set_type                     = std::unordered_set<cell_instance_type, entity_system::EntityBaseHash>;
            using gcell_set_type                    = std::unordered_set<gcell_type, entity_system::EntityBaseHash>;
            using gcell_cell_instances_type         = entity_system::Property<gcell_type, cell_set_type>;
            using gcell_graph_ptr_type              = std::shared_ptr<ophidian::routing::GCellGraph>;
            using layer_type                        = Layer;
            using flute_graph_type                  = lemon::ListGraph;
            using flute_node_type                   = lemon::ListGraph::Node;
            using node_map_type                     = flute_graph_type::NodeMap<FluteNode>;
            using edge_map_type                     = flute_graph_type::EdgeMap<std::vector<gcell_type>>;
            using routing_segment_type              = std::pair<gcell_type, gcell_type>;

            AStarRouting(design_type & design);

            bool route_net(const net_type & net, std::vector<AStarSegment> & segments, bool applying_routing = true);
            bool apply_segments_to_global_routing(const std::vector<AStarSegment> & segments);
        private:
            bool init_flute_graph();
            bool init_two_pin_flute_graph();
            bool node_layer_assignment();
            bool route_flute_segments();
            bool a_star(flute_node_type start, flute_node_type goal);
            unsigned int heuristic(const gcell_type & source, const gcell_type & goal, bool goal_is_steiner) const;
            std::vector<gcell_type> neighbors(gcell_type gcell);
            void update_f_score(gcell_type gcell, gcell_type neighbor_gcell, gcell_type goal_gcell, bool goal_is_steiner);
            bool goal_reached(const gcell_type & source, const gcell_type & goal, bool goal_is_steiner) const;
            void bfs_backtrack();//TODO change this function to be a BFS visitor
            void back_track_path(flute_node_type s, flute_node_type g);
            void gcell_path_to_routing_segments(flute_node_type s, flute_node_type g);
            bool connect_pins_to_min_layer();
            void generate_routing_segments(std::vector<AStarSegment> & segments);
            void clear_router_members();
            bool connect_floating_pins();
            bool all_pins_same_collumn();
            bool trivial_routing();


            net_type                              m_net;
            flute_graph_type                      m_graph;
            flute_node_type                       m_root_node;
            node_map_type                         m_node_map;
            edge_map_type                         m_edge_map;
            gcell_property_type                   m_gcell_to_AStarNode;
            design_type&                          m_design;
            gcell_graph_ptr_type                  m_gcell_graph;
            layer_type                            m_min_layer;
            std::vector<routing_segment_type>     m_routing_segments;
            gcell_extra_demand_type               m_same_gcell_extra_demand;
            gcell_extra_demand_type               m_adj_gcell_extra_demand;
            gcell_cell_instances_type             m_gcells_cell_instances;//This could be in GCellGraph class.
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
