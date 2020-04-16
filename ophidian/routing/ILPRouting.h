#ifndef OPHIDIAN_ILP_ROUTING
#define OPHIDIAN_ILP_ROUTING

#include <unordered_set>

#include <boost/geometry/index/rtree.hpp>

#include <ophidian/geometry/Models.h>

#include <ophidian/design/Design.h>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/routing/Library.h>

#include "gurobi_c++.h"

namespace ophidian::routing {
    class RouteCandidate : public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class NetSegment : public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class NetWire : public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class ILPRouting {
        public:
            using unit_type                 = util::database_unit_t;
            using design_type               = design::Design;
            using net_type                  = circuit::Net;
            using cell_type                 = circuit::CellInstance;
            using pin_type                  = circuit::Pin;
            using pin_container_type        = std::vector<pin_type>;
            using segment_type              = NetSegment;
            using candidate_type            = RouteCandidate;
            using wire_type                 = NetWire;
            using wire_container_type       = std::vector<wire_type>;
            using gcell_type                = GCell;

            using point_type                = util::LocationDbu;
            using ilp_var_type              = GRBVar;
            using ilp_var_container_type    = std::vector<ilp_var_type>;

            using unitless_point_type       = geometry::Point<double>;
            using unitless_box_type         = geometry::Box<double>;
            using unitless_segment_type     = geometry::Segment<unitless_point_type>;

            using rtree_node_type           = std::pair<unitless_box_type, gcell_type>;
            using rtree_type                = boost::geometry::index::rtree<rtree_node_type, boost::geometry::index::rstar<16>>;

            using layer_type                = Library::layer_type;
            using layer_direction_type      = Direction;

            ILPRouting(design_type & design, std::string circuit_name);

            bool route_nets(const std::vector<net_type> & nets);

        private:
            void update_gcell_capacities();

            void create_gcell_rtrees();

            void create_all_candidates(const std::vector<net_type> & nets, GRBModel & model);

            void create_net_candidates(const net_type & net, GRBModel & model);

            void create_net_candidates_in_layers(const net_type & net, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool large_net, const std::set<std::pair<unit_type, unit_type>> & steiner_points, GRBModel & model);

            void add_wires_of_splitted_segment(const segment_type & segment, const point_type & segment_start, const point_type & segment_end, const layer_type & horizontal_layer, const layer_type & vertical_layer, bool connect_on_y, unsigned branch_count, const std::vector<candidate_type> & candidates, bool large_net);

            void create_candidate(const net_type & net, const wire_container_type & wires, std::string variable_name, GRBModel & model);

            void add_wires_to_candidate(const candidate_type & candidate, const wire_container_type & wires);

            void split_segment(const point_type & segment_start, const point_type & segment_end, const layer_type & wire1_layer, const layer_type & wire2_layer, bool connect_on_y, wire_container_type & wires);

            wire_type create_wire(const point_type & wire_start, const point_type & wire_end, const layer_type & start_layer, const layer_type & end_layer);

            void add_objective_function(GRBModel & model);

            void add_candidate_constraints(const std::vector<net_type> & nets, GRBModel & model);

            void add_capacity_constraints(const std::vector<net_type> & nets, GRBModel & model);

	        void write_gcell_capacities();

	        void write_segments(const std::vector<net_type> & nets);

            void draw_gcell_capacities(boost::geometry::svg_mapper<unitless_point_type> & mapper);

            void write_svg(const std::vector<net_type> & nets);

    	    void save_result();

            design_type&                                                    m_design;

            GRBEnv                                                          m_GRBENv;

            entity_system::EntitySystem<wire_type>                          m_wires;
            entity_system::Property<wire_type, point_type>                  m_wire_starts{m_wires};
            entity_system::Property<wire_type, point_type>                  m_wire_ends{m_wires};
            entity_system::Property<wire_type, layer_type>                  m_wire_start_layers{m_wires};
            entity_system::Property<wire_type, layer_type>                  m_wire_end_layers{m_wires};

            entity_system::EntitySystem<candidate_type>                     m_candidates;
            entity_system::Property<candidate_type, std::string>            m_candidate_names{m_candidates};
            entity_system::Property<candidate_type, ilp_var_type>           m_candidate_variables{m_candidates};
            entity_system::Property<candidate_type, unit_type>              m_candidate_wirelengths{m_candidates};
            //entity_system::Aggregation<candidate_type, wire_type>         m_candidate_wires{m_candidates, m_wires};
    	    entity_system::Property<candidate_type, wire_container_type>    m_candidate_wires{m_candidates};

            std::unordered_map<std::string, candidate_type>                 m_name_to_candidate;

            entity_system::EntitySystem<segment_type>                       m_segments;
            entity_system::Property<segment_type, point_type>               m_segment_starts{m_segments};
            entity_system::Property<segment_type, point_type>               m_segment_ends{m_segments};
            entity_system::Property<segment_type, pin_container_type>       m_segment_start_pin{m_segments};
            entity_system::Property<segment_type, pin_container_type>       m_segment_end_pin{m_segments};
            //entity_system::Aggregation<segment_type, candidate_type>      m_segment_candidates{m_segments, m_candidates};

            entity_system::Aggregation<net_type, candidate_type>            m_net_candidates{m_design.netlist().make_aggregation_net<candidate_type>(m_candidates)};
            entity_system::Aggregation<net_type, segment_type>              m_net_segments{m_design.netlist().make_aggregation_net<segment_type>(m_segments)};

            std::vector<rtree_type>                                         m_gcells_rtrees;

	        std::string                                                     m_circuit_name;
    };
}

#endif
