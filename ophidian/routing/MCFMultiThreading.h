#ifndef MCF_MULTI_THREADING_H
#define MCF_MULTI_THREADING_H

// https://www.bfilipek.com/2019/12/threading-loopers-cpp17.html
// Using looper with Dispatcher in C++17

// Questions:
//      Are things executed in proper order?
//      Who maintains the threads?
//      How to receive results from asynchronous execution?
//      What about task priorities or delayed insertions?
//      Maybe even event-driven dispatching?    

// Looper definition:
// Loopers, in its core, are objects, which contain or are attached to a thread with a conditional infinite loop, which runs as long as the abort-criteria is unmet. Within this loop, arbitrary actions can be performed.
// Usually, a methodology like start, run and stop are provided.


// #include <thread>
// #include <atomic>
// #include <memory>
// #include <functional>
// #include <stdexcept>
// #include <iostream>
// #include <mutex>
// #include <queue>

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
// #include <omp.h>

using namespace std;

// #define NUM_THREADS 5
//#include <ophidian/routing/CLooper.h>
// typedef void * (*THREADFUNCPTR)(void *);

namespace UCal{

class AstarResult{
    using net_type                  = ophidian::circuit::Net;
    public: 
    net_type m_net;
    std::vector<ophidian::routing::AStarSegment> m_initial_segments;
    std::vector<ophidian::routing::AStarSegment> m_astar_segments;
    bool m_is_valid;
};//end AStarResutls

class MoveCellResult{
    using net_type                  = ophidian::circuit::Net;
    using gcell_type = ophidian::routing::GlobalRouting::gcell_type;
    public: 
    std::vector<net_type> m_nets;
    std::vector<ophidian::routing::AStarSegment> m_initial_segments;
    std::vector<ophidian::routing::AStarSegment> m_astar_segments;
    int m_astar_wl;
    gcell_type m_init_gcell;
    gcell_type m_target_gcell;
    std::string m_cell_name;
    bool m_is_valid;
};//end AStarResutls
    

class MCFMultiThreading{
    public: 
    using design_type               = ophidian::design::Design;
    using net_type                  = ophidian::circuit::Net;
    using point_type                = ophidian::util::LocationDbu;
    using gcell_type = ophidian::routing::GlobalRouting::gcell_type;
    using AStarSegment = ophidian::routing::AStarSegment;
    
     MCFMultiThreading(design_type & design);
    ~MCFMultiThreading();
    void run(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
    private: 
        void construct_net_boxes_rtree(const std::vector<net_type> &nets);
        void cluster_based_on_nets_box();
        
        void write_nets(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void cluster_based_on_panel();
        //vertical and horizontal paneling
        void cluster_based_on_panel_v2();
        void run_ilp_on_panels(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_ilp_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_astar_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_astar_on_panels_parallel_v2(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_ilp_on_panel(unsigned int panel_id,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        
        void run_astar_on_panel(unsigned int panel_id);
        void update_global_routing();
        void run_ilp(ophidian::placement::Placement::box_type panel_region,std::set<std::string> nets_set,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        AstarResult run_astar_on_net(net_type& net);
        void update_astar_on_global_routing(AstarResult& astar_result);
        void data_analysis(std::string file_name);

        void report();
        void reportPanelIds();

        //Astar Movement
        std::vector<std::pair<ophidian::circuit::CellInstance, double>> compute_cell_move_costs_descending_order(ophidian::design::Design & design);
        void run_compute_cell_move_costs_parallel();
        void run_compute_cell_move_costs_on_panel(unsigned int panel_id);
        bool cell_has_more_than_1_pin_in_same_net(ophidian::design::Design & design, ophidian::circuit::CellInstance cell);
        void move_cells_for_until_x_minutes(ophidian::design::Design & design,
                                    int time_limit,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements,
                                    ophidian::routing::AStarRouting & astar_routing);
        MoveCellResult move_cell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, ophidian::routing::AStarRouting & astar_routing);                                    
        void calculate_median_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, std::vector<gcell_type> & target_gcells);
        
        MoveCellResult test_target_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, gcell_type & initial_gcell, gcell_type & target_gcell, ophidian::routing::AStarRouting & astar_routing);
        void move_cells_parallel(std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements);
        void run_move_cell_on_panel(unsigned int panel_id,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements);
        void move_cells_parallel_v2(std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements);
        void move_cells_parallel_v3(std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements);

        design_type&    m_design;

        // keep the box of each net inside a rtree
        RTreeDB m_rtree_net_box;

        // map id net_box to net_name
        std::unordered_map<unsigned int, std::string> m_index_net_box_to_net_name_dict;
        // string net_name to net_type
        std::unordered_map<std::string, net_type> m_net_name_to_net_type_dict;

        std::vector<ophidian::circuit::Net> m_routed_nets;

        // Paneling variables 
        std::vector<std::vector<Box2DWrapper>> m_panel_boxes; 

        RTreeDB m_rtree_panel_box;
        std::unordered_map<unsigned int, Box2DWrapper> m_index_to_panel;

        std::unordered_map<unsigned int,std::set<std::string>> m_panel_index_to_nets_dict;

        // each panel wire length
        std::unordered_map<unsigned int, int> m_panel_wirelength_dict;

        // panel index to wl after ilp
        std::unordered_map<unsigned int, int> m_panel_wirelength_after_ilp_dict;

        std::vector<ophidian::circuit::Net> m_fixed_nets;

        //panel level to panel index
        std::map<unsigned int,std::vector<unsigned int>> m_panel_level;

        int m_total_panel_nets; 

        std::vector<ophidian::routing::ILPResult> m_ilp_results;
        ophidian::routing::AStarRouting m_astar_routing{m_design};
        std::unordered_map<unsigned int, std::set<std::string>> m_panel_index_to_local_cells_dict;
        // std::vector<std::pair<net_type,box_type>> m_routed_nets;
        // std::vector<net_type> m_unrouted_nets;
        //ophidian::routing::AStarRouting astar_routing{m_design};

        //net name to box
        std::unordered_map<std::string,Box2DWrapper> m_net_name_to_net_box_dict;

        int get_wire_length_segments(const std::vector<AStarSegment> & segments);

        ofstream net_astar_file;

        std::vector<MoveCellResult> m_move_cells;
        std::unordered_map<std::string,ophidian::circuit::CellInstance> m_cell_name_to_cell_type_dict;
 
};//end class 

};//end namespace 

#endif
