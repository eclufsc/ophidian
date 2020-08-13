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

#define NUM_THREADS 5
//#include <ophidian/routing/CLooper.h>
// typedef void * (*THREADFUNCPTR)(void *);

namespace UCal{
    

class MCFMultiThreading{
    public: 
    using design_type               = ophidian::design::Design;
    using net_type                  = ophidian::circuit::Net;
    using point_type                = ophidian::util::LocationDbu;
    
     MCFMultiThreading(design_type & design);
    ~MCFMultiThreading();
    void run(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
    private: 
        void construct_net_boxes_rtree(const std::vector<net_type> &nets);
        void cluster_based_on_nets_box();
        void write_nets(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void cluster_based_on_panel();
        void run_ilp_on_panels(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_ilp_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_astar_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_ilp_on_panel(unsigned int panel_id,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void run_astar_on_panel(unsigned int panel_id);
        void update_global_routing();
        void run_ilp(ophidian::placement::Placement::box_type panel_region,std::set<std::string> nets_set,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements);
        void data_analysis(std::string file_name);

        void report();
        void reportPanelIds();

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
        // std::vector<std::pair<net_type,box_type>> m_routed_nets;
        // std::vector<net_type> m_unrouted_nets;
        //ophidian::routing::AStarRouting astar_routing{m_design};

 
};//end class 

};//end namespace 

#endif
