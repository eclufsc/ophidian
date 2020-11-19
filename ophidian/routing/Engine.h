#ifndef ENGINE_H
#define ENGINE_H

#include <ophidian/design/Design.h>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/routing/Library.h>
#include <armadillo>
#include "MCFRoutingRTree.h"
//#include "MCFRouting.h"
#include "ILPRouting.h"
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/routing/AStarRouting.h>
#include "json.hpp"

using namespace std;

namespace UCal{
    typedef nlohmann::json Json;

    class AstarResultV2{
        using net_type                  = ophidian::circuit::Net;
        public: 
        net_type m_net;
        std::vector<ophidian::routing::AStarSegment> m_initial_segments;
        std::vector<ophidian::routing::AStarSegment> m_astar_segments;
        bool m_is_valid;
    };//end AStarResutls

    class Engine{
        
        public: 
            using design_type               = ophidian::design::Design;
            using net_type                  = ophidian::circuit::Net;
            using point_type                = ophidian::util::LocationDbu;
            using cell_type                 = ophidian::circuit::CellInstance;
            using gcell_type = ophidian::routing::GlobalRouting::gcell_type;
            using AStarSegment = ophidian::routing::AStarSegment;

            using movement_container_type = std::unordered_map< cell_type, point_type, ophidian::entity_system::EntityBaseHash>; 


            Engine(design_type & design);
            ~Engine();
            Json run(movement_container_type & movements,std::chrono::steady_clock::time_point start_time);
            // void run_date21(std::chrono::steady_clock::time_point start_time);
            std::set<std::string>& get_nets_in_panels(){return m_nets_inside_panels;}
            int get_wire_length_segments(const std::vector<AStarSegment> & segments);
            void run_astar_on_circuit(const std::vector<ophidian::circuit::Net> & nets);
        private: 
            void construct_net_boxes_rtree(const std::vector<net_type> &nets);
            void cluster_based_on_panel();
            void run_astar_on_panels_sequential(int remaining_time); 
            void run_astar_on_panels_parallel_loop(int remaining_time);           
            void run_astar_on_panels_parallel_section(int remaining_time); 
            void run_astar_on_panel(unsigned int panel_id);
            AstarResultV2 run_astar_on_net(net_type& net);
            void update_astar_on_global_routing(AstarResultV2& astar_result);
            void loadParams();
            void update_global_routing();
            void run_ilp_on_panels_parallel(movement_container_type & movements);
            void run_ilp_on_panel(unsigned int panel_id,movement_container_type & movements);
            void run_ilp(ophidian::placement::Placement::box_type panel_region,std::set<std::string> nets_set,movement_container_type & movements);
            


            design_type&    m_design;
            // keep the box of each net inside a rtree
            RTreeDB m_rtree_net_box;
            RTreeDB m_rtree_panel_box;
            std::unordered_map<unsigned int, Box2DWrapper> m_index_to_panel;

            std::unordered_map<unsigned int,std::set<std::string>> m_panel_index_to_nets_dict;

            //panel level to panel index
            std::map<unsigned int,std::vector<unsigned int>> m_panel_level;

            // string net_name to net_type
            std::unordered_map<std::string, net_type> m_net_name_to_net_type_dict;
            std::set<std::string> m_nets_inside_panels;

            // map id net_box to net_name
            std::unordered_map<unsigned int, std::string> m_index_net_box_to_net_name_dict;

            //net name to box
            std::unordered_map<std::string,Box2DWrapper> m_net_name_to_net_box_dict;

            ophidian::routing::AStarRouting m_astar_routing{m_design};
            double m_temperature;


            int m_total_panel_nets;
            std::vector<ophidian::routing::ILPResult> m_ilp_results;

            Json m_global_params;
            Json m_timer_report_json;


              
    };//end Engine Class 


        

};//end namespace 

#endif
