#ifndef EXPERIMENTS_ASTAR_COLAPSE
#define EXPERIMENTS_ASTAR_COLAPSE

#include <ophidian/routing/AStarRouting.h>
#include <ophidian/design/Design.h>

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Aggregation.h>

#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>
#include "run_ilp.h"

class AstarColapseNet{
    public:
        using scalar_type               = int;
        using unit_type                 = ophidian::util::database_unit_t;

        using point_type                = ophidian::util::LocationDbu;
        using point_container_type      = std::vector<point_type>;

        using design_type               = ophidian::design::Design;
        using net_type                  = ophidian::circuit::Net;
        using net_container_type        = std::vector<net_type>;
        using cell_type                 = ophidian::circuit::CellInstance;
        using pin_type                  = ophidian::circuit::PinInstance;
        using pin_container_type        = std::vector<pin_type>;
        using gcell_type                = ophidian::routing::GCell;
        using gcell_container_type      = std::vector<gcell_type>;

        using a_star_segment_type       = ophidian::routing::AStarSegment;
        using cell_set_type             = std::unordered_set<cell_type, ophidian::entity_system::EntityBaseHash>;
        using net_set_type              = std::unordered_set<net_type, ophidian::entity_system::EntityBaseHash>;

        AstarColapseNet(design_type & design);

        void colapse_nets(net_container_type & nets, std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements);
    private:

        design_type & m_design;
        ophidian::routing::GlobalRouting & m_global_routing;
        ophidian::circuit::Netlist & m_netlist;
        ophidian::placement::Placement & m_placement;

        ophidian::routing::AStarRouting m_astar_routing;

        ophidian::entity_system::Property<net_type, cell_set_type>  m_net_cells{m_design.netlist().make_property_net<cell_set_type>()};
        ophidian::entity_system::Property<net_type, net_set_type>   m_net_neighbors{m_design.netlist().make_property_net<net_set_type>()};
        ophidian::entity_system::Property<net_type, point_type>     m_net_median{m_design.netlist().make_property_net<point_type>()};

        void save_initial_state(
                const ophidian::circuit::Net & net,
                std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
                std::vector<ophidian::routing::AStarSegment> & initial_segments);
        
        void restore_initial_status(
                const std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & initial_cells_location,
                const std::vector<ophidian::routing::AStarSegment> & initial_segments);

        /*
        return 1  -> net moved
        return 0  -> net not moved
        return -1 -> net not moved because movement is not efficient
        */
        int move_net(ophidian::circuit::Net & net, std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements);
};


#endif