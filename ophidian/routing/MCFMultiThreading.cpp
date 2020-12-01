#include "MCFMultiThreading.h"

#include <algorithm>
#include <set>
#include <iterator>
#include <fstream>

namespace UCal{

bool DEBUG_MCF_MLT_NET_BOX_RTREE = false;
bool DEBUG_MCF_MLT_CHECK_ROWS = false;
bool DEBUG_MCF_MLT_SPECIFIC_NET = false;
// log in each row how many nets are intescted, how many are inside the panel
// how many can be routed and reduced wire length
bool DEBUG_MCF_MLT_NETS_ROWS = false;
bool DEBUG_PANEL = false;
bool DEBUG_PANEL_PARALLEL = false;
bool DEBUG_ASTAR_PARALLEL = false;

#define num_nets_to_route 10000
#define WINDOW_SIZE 100 // means each panels covers 10 gcells in y-axis

MCFMultiThreading::MCFMultiThreading(design_type & design):
    m_design(design)
 //:
    // mDispatcher(std::shared_ptr<CDispatcher>(new CDispatcher(*this)))
{
    if(DEBUG_ASTAR_PARALLEL) net_astar_file.open("net_astar_file");
    if(DEBUG_ASTAR_PARALLEL) net_astar_file  << "net_name,init_wl,astar_wl\n";
}//end MCFMultiThreading constructor

MCFMultiThreading::~MCFMultiThreading(){
    // abortAndJoin();
}//end MCFMultiThreading destructor

void MCFMultiThreading::run(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    //std::vector<std::pair<ophidian::routing::ILPRoung<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;
    auto start_time = std::chrono::steady_clock::now();

    m_design.placement().reset_rtree();
    

    // std::set<std::string> s1;
    // std::set<std::string> s2;

    // s1.insert("net_1");
    // s1.insert("net_2");
    // s1.insert("net_1");
    // s2.insert("net_1");

    // for(auto str : s1){
    //     std::cout << str << std::endl;
    // }


    // std::set<std::string> result;
    // std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),std::inserter(result, result.end()));
    // std::cout << "result: "<< std::endl;
    
    // for(auto str : result){
    //     std::cout << str << std::endl;
    // }

    // return;


    construct_net_boxes_rtree(nets);
    // if(!DEBUG_MCF_MLT_SPECIFIC_NET)
    //     cluster_based_on_nets_box();

    cluster_based_on_panel_v2();
    //data_analysis("before");
    // run_ilp_on_panels(movements);
    // run_ilp_on_panel(1,movements);

    m_design.global_routing().set_gcell_cell_instances(m_design.netlist(), m_design.placement());
    run_ilp_on_panels_parallel(movements);
    log() << "run astar on panels" << std::endl;
    run_astar_on_panels_parallel(movements);
    if(DEBUG_ASTAR_PARALLEL) net_astar_file.close();
    return;   

    

    log() << "movements after ILP " << movements.size() << std::endl;
    
    auto end_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end_time-start_time;
    auto current_time = diff.count() / 60.0;
    auto remaining_time = 55 - current_time;

    log() << "current time " << current_time << " remaining time " << remaining_time << std::endl;
    
    for (auto movement : movements) {
        auto cell = movement.first;
        m_design.placement().fixLocation(cell);
    }


    //compute lower bound using A* without capacities!
    ophidian::routing::AStarRouting astar_routing{m_design};
    /*printlog();
    comput_lower_bound(design, astar_routing);
    printlog();*/
    log() << "compute_cell_move_costs_descending_order "  << std::endl;
    run_compute_cell_move_costs_parallel();
    // auto cell_costs = compute_cell_move_costs_descending_order(m_design);

    /*auto debug_gcell = design.global_routing().gcell_graph()->gcell(23, 10, 0);
    auto capacity = design.global_routing().gcell_graph()->capacity(debug_gcell);
    auto demand = design.global_routing().gcell_graph()->demand(debug_gcell);
    auto layer_index = design.global_routing().gcell_graph()->layer_index(debug_gcell);
    auto gcell_box = design.global_routing().gcell_graph()->box(debug_gcell);

    std::cout << "debug gcell before moves " << gcell_box.min_corner().y().value() << " " << gcell_box.min_corner().x().value() << " " << layer_index << std::endl;
    std::cout << "debug gcell before moves capacity " << capacity << " demand " << demand << std::endl;*/
    // log() << "move_cells_parallel "  << std::endl;
    // move_cells_parallel(movements);
    // log() << "number of movements: " << movements.size() << std::endl;
    // move_cells_for_until_x_minutes(m_design, remaining_time, cell_costs, movements, m_astar_routing);
    // move_cells_parallel_v2(cell_costs,movements);

}//end run function


void MCFMultiThreading::construct_net_boxes_rtree(const std::vector<net_type> &nets){
    auto & netlist = m_design.netlist();
    auto & placement = m_design.placement();    
    auto& global_routing = m_design.global_routing();
    auto& routing_library = m_design.routing_library();
        
    auto graph_ptr = global_routing.gcell_graph();

    //find net boxes
    unsigned int net_index = 1;
    for(auto net : nets){

        /*if(DEBUG_MCF_MLT_SPECIFIC_NET){
            auto net_name = m_design.netlist().name(net);
            std::vector<net_type> nets_local;
            // if(net_name == "N1363" || net_name == "N1"){
            if( net_name == "N1"){
                nets_local.push_back(net);
                MCFRouting mcf_routing(m_design,nets_local,"case");
                mcf_routing.route_nets();
            }else{
                continue;
            }
                
        }*/
            

        std::vector<int> location_xs;
        std::vector<int> location_ys;
        auto net_name = m_design.netlist().name(net);
        if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "net_name: " << net_name << std::endl;


        // for(auto net_pin : netlist.pins(net)){

        //     auto location = placement.location(net_pin);               
        //     location_xs.push_back(location.x().value());
        //     location_ys.push_back(location.y().value());
        //     if(DEBUG_MCF_MLT_NET_BOX_RTREE)  std::cout << location.x().value() <<", " << location.y().value() << std::endl;
        //     //std::cout << "pin location " << location.x().value() << ", " << location.y().value() << std::endl;
        // }//end for 
        auto segments = m_design.global_routing().segments(net);

        if(segments.size() > 0)
        {
            for(auto segment : segments){
                auto start_gcell = global_routing.gcell_start(segment);
                auto start_box = graph_ptr->box(start_gcell);
                point_type start{(start_box.min_corner().x()+start_box.max_corner().x())/2,
                                 (start_box.min_corner().y()+start_box.max_corner().y())/2};
                auto end_gcell = global_routing.gcell_end(segment);
                auto end_box = graph_ptr->box(end_gcell);
                point_type end{(end_box.min_corner().x()+end_box.max_corner().x())/2,
                               (end_box.min_corner().y()+end_box.max_corner().y())/2};

                location_xs.push_back(start.x().value());
                location_ys.push_back(start.y().value());
                location_xs.push_back(end.x().value());
                location_ys.push_back(end.y().value());

            }//end for 

            std::vector<int>::iterator min_x_itrerator = std::min_element(location_xs.begin(), location_xs.end());
            std::vector<int>::iterator max_x_itrerator = std::max_element(location_xs.begin(), location_xs.end());
            std::vector<int>::iterator min_y_itrerator = std::min_element(location_ys.begin(), location_ys.end());
            std::vector<int>::iterator max_y_itrerator = std::max_element(location_ys.begin(), location_ys.end());
            int min_x = *min_x_itrerator;
            int max_x = *max_x_itrerator;
            int min_y = *min_y_itrerator;
            int max_y = *max_y_itrerator;
            if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "box: " << min_x << ", " << min_y << ", " << max_x << ", " << max_y << std::endl;

            Box2DWrapper net_box(min_x,min_y,max_x,max_y);
            unsigned int net_box_index = net_index;
            m_index_net_box_to_net_name_dict[net_box_index] = net_name;
            m_rtree_net_box.insert(net_box,net_index);
            m_net_name_to_net_type_dict[net_name] = net;
            m_net_name_to_net_box_dict[net_name] = net_box;
            net_index++;
        }//end if 





        
    }//end for 
}//end construct_net_boxes_rtree


void MCFMultiThreading::cluster_based_on_nets_box(){
    
    auto chip_upper_right_corner = m_design.floorplan().chip_upper_right_corner();
    // The window is 10Gcellx width of layout
    int window_size = WINDOW_SIZE;
    
    int layout_width = chip_upper_right_corner.x().value();
    int layout_height = chip_upper_right_corner.y().value();

    int min_x_window = 1;
    int min_y_window = 1;
    int max_x_window = layout_width;
    int max_y_window = window_size;
    Box2DWrapper window(1,1,layout_width,window_size);
    int number_of_windows = (layout_height/window_size)+1;


    

    if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "chip_upper_right_corner: " << chip_upper_right_corner.x().value() << ", " << chip_upper_right_corner.y().value() << std::endl;

    
        
    // number_of_windows = 1;
    for(int i = 0; i < number_of_windows; i++){
        min_x_window = 0;//i*window_size;
        min_y_window = i*window_size;
        max_x_window = layout_width;
        max_y_window = min_y_window + window_size;
        Box2DWrapper window_local(min_x_window,min_y_window,max_x_window,max_y_window);

        if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "row_" << i << ": " << min_x_window << ", " << min_y_window << ", " << max_x_window << ", " << max_y_window << std::endl;
        if(DEBUG_MCF_MLT_NETS_ROWS)  std::cout << "row_" << i << ": " << min_x_window << ", " << min_y_window << ", " << max_x_window << ", " << max_y_window << std::endl;
        // if(DEBUG_MCF_MLT_CHECK_ROWS) std::cout << "row_" << i << ": " << min_x_window << ", " << min_y_window << ", " << max_x_window << ", " << max_y_window << std::endl;

        auto values = m_rtree_net_box.query(window_local);
        
        std::vector<net_type> nets_local;
        // std::vector<std::string> nets_local_boost;
        
        for (auto value : values){
            auto net_inex =value.second;
            auto net_box = value.first;
            auto net_name = m_index_net_box_to_net_name_dict[net_inex];
            Box2DWrapper net_box_wrapper(net_box);

            if(DEBUG_MCF_MLT_NET_BOX_RTREE){
                std::cout << "value index: " << net_inex<< std::endl;
                std::cout << "net_name: " << net_name << std::endl;
                net_box_wrapper.report();     
            }
            
               

            if((net_box_wrapper.getXl() >= window_local.getXl()) &&
               (net_box_wrapper.getYl() >= window_local.getYl()) &&
               (net_box_wrapper.getXh() <= window_local.getXh()) &&
               (net_box_wrapper.getYh() <= window_local.getYh())
            ){
                if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "is inside window local: ";
                if(DEBUG_MCF_MLT_NET_BOX_RTREE) window_local.report();


                    
                
                nets_local.push_back(m_net_name_to_net_type_dict[net_name]);
            }

            // if(boost::geometry::within(net_box,window_local.getBox())){
            //     std::cout << "is inside window local boost: ";
            //     window_local.report();

            //     nets_local_boost.push_back(net_name);
            // }//end if 
        }//end for 

        if(DEBUG_MCF_MLT_CHECK_ROWS) {
            std::cout << "intersected, inside\n";
            std::cout << values.size() << ", " << nets_local.size()<< std::endl;
        }
        if(DEBUG_MCF_MLT_NETS_ROWS) {
            std::cout << "intersected: " << values.size() << ", " << " inside: " << nets_local.size() << std::endl;
        }
        // std::cout << "number of nets inside window boost: " << nets_local_boost.size() << std::endl;

        // MCFRouting mcf_routing(m_design,nets_local,"case");
        // mcf_routing.route_nets();

        std::vector<ophidian::circuit::Net> fixed_nets;
        std::vector<ophidian::circuit::Net> routed_nets;
        std::vector<ophidian::circuit::Net> unrouted_nets;

        // std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N2003")};
        // std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N2116")};

        std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;
        if(DEBUG_MCF_MLT_NETS_ROWS) std::cout << "routing nets" << std::endl;
        ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
        /*auto result = ilpRouting.route_nets(nets_local, {}, fixed_nets, routed_nets, unrouted_nets, movements);

        if(DEBUG_MCF_MLT_NETS_ROWS) std::cout << "result " << result.first << std::endl;*/

        for(auto net_tmp : routed_nets){
            m_routed_nets.push_back(net_tmp);
        }
       
    }//end for 

    

}//end cluster_based_on_nets_box


void MCFMultiThreading::write_nets(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
        
        ophidian::parser::ICCAD2020Writer iccad_output_writer(m_design, "case");
        if(DEBUG_MCF_MLT_NETS_ROWS) std::cout << "number of movement: " << movements.size() << std::endl;
        if(DEBUG_PANEL) std::cout << "number of movement: " << movements.size() << std::endl;
        // if(result.first){
        if(true){
            iccad_output_writer.write_ICCAD_2020_output("mcf_output.txt", movements);
        }
   
        //write_statistics_for_circuit(design, circuit_name);

        // std::cout << "connected nets, m_routed_nets: " << m_routed_nets.size() << std::endl;
        // for (auto net : m_routed_nets) {
        //     ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
        //     for (auto pin : m_design.netlist().pins(net)) {
        //         auto pin_name = m_design.netlist().name(pin);                
        //         auto location = m_design.placement().location(pin);
        //         auto box = ophidian::routing::GCellGraph::box_type{location, location};
        //         auto pin_geometry = m_design.placement().geometry(pin);
        //         auto layer_name = pin_geometry.front().second;
        //         auto pin_layer = m_design.routing_library().find_layer_instance(layer_name);
        //         auto layer_index = m_design.routing_library().layerIndex(pin_layer);

        //         // std::cout << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

        //         m_design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
        //     }//end for 
        //     auto net_name = m_design.netlist().name(net);
        //     auto connected = m_design.global_routing().is_connected(net, pin_gcells, net_name);

        //     if(!connected)
        //         std::cout << "net " << net_name << " is open" << std::endl;
        // } //end for 

}//end write_nets

void MCFMultiThreading::cluster_based_on_panel(){
    auto chip_upper_right_corner = m_design.floorplan().chip_upper_right_corner();
    // The window is 10Gcellx width of layout
    int panel_base = 100;
    
    int layout_width = chip_upper_right_corner.x().value();
    int layout_height = chip_upper_right_corner.y().value();

    int panel_min_x = 1;
    int panel_min_y = 1;
    int panel_max_x = layout_width;
    int panel_max_y = panel_base;
    // Box2DWrapper panel_box(1,1,layout_width,panel_base);
    int number_of_panels = (layout_height/panel_base)+1;


    int count = 1;
    int total_panels = 0;
    unsigned int panel_level = 0;

    while(number_of_panels >= 1){
        if(DEBUG_PANEL) std::cout << "level: " << count << std::endl;
        auto panel_size = panel_base * count;
        number_of_panels = ceil(layout_height/panel_size);
        // std::vector<Box2DWrapper> panels;
        panel_level++;
        std::vector<unsigned int> panel_ids;
        for (int panel_iterator = 0; panel_iterator <= number_of_panels ; panel_iterator++){
            // compute panel corner 
            panel_min_x = 0;//i*window_size;
            panel_min_y = panel_iterator*panel_size;
            panel_max_x = layout_width;
            panel_max_y = panel_min_y + panel_size;
            Box2DWrapper panel_box_current(panel_min_x,panel_min_y,panel_max_x,panel_max_y);
            unsigned int panel_index = total_panels;
            total_panels++;
            // panels.push_back(panel_box_current);

            if(DEBUG_PANEL) std::cout << "row_" << panel_iterator << ": " 
                          << panel_min_x << ", "
                          << panel_min_y << ", "
                          << panel_max_x << ", "
                          << panel_max_y << std::endl;
        

            auto nets_intersected_with_panel_pair = m_rtree_net_box.query(panel_box_current);
            
            std::vector<std::string> nets_name_inside_panel;
            std::set<std::string> nets_inside_intesected_panels;
            for (auto net_intersect_with_panel_pair : nets_intersected_with_panel_pair){
                auto net_inex = net_intersect_with_panel_pair.second;
                auto net_box  = net_intersect_with_panel_pair.first;
                auto net_name = m_index_net_box_to_net_name_dict[net_inex];
                Box2DWrapper net_box_wrapper(net_box);

                // if(DEBUG_MCF_MLT_NET_BOX_RTREE){
                //     std::cout << "value index: " << net_inex<< std::endl;
                //     std::cout << "net_name: " << net_name << std::endl;
                //     net_box_wrapper.report();     
                // }
                
                

                if((net_box_wrapper.getXl() >= panel_box_current.getXl()) &&
                   (net_box_wrapper.getYl() >= panel_box_current.getYl()) &&
                   (net_box_wrapper.getXh() <= panel_box_current.getXh()) &&
                   (net_box_wrapper.getYh() <= panel_box_current.getYh())
                ){
                    nets_name_inside_panel.push_back(net_name);
                }
            }//end for 

            Box2DWrapper panel_box_current_polish( panel_box_current.getXl(),
                                                    panel_box_current.getYl()+1,
                                                    panel_box_current.getXh(),
                                                    panel_box_current.getYh()-1);
            auto panels_intersect_with_current_panel_pair = m_rtree_panel_box.query(panel_box_current_polish);
            if(DEBUG_PANEL ) std::cout << "panels_intersect_with_current_panel_pair size: " << panels_intersect_with_current_panel_pair.size() << std::endl;
            for(auto panels_intersect_with_current_panel_pair : panels_intersect_with_current_panel_pair){
                auto nets_inside_intesected_panel = m_panel_index_to_nets_dict[panels_intersect_with_current_panel_pair.second];
                for(auto net_inside_intesected_panel: nets_inside_intesected_panel){
                    nets_inside_intesected_panels.insert(net_inside_intesected_panel);
                }//end for 
            }//end for 

                


            
            std::set<std::string> nets_name_inside_panel_polished;
            for(auto net : nets_name_inside_panel){
                if(nets_inside_intesected_panels.find(net) == nets_inside_intesected_panels.end())
                    nets_name_inside_panel_polished.insert(net);
            }
            // std::set_difference(nets_name_inside_panel.begin(), nets_name_inside_panel.end(),
            //                  nets_inside_intesected_panels.begin(), nets_inside_intesected_panels.end(),
            //                  std::inserter(nets_name_inside_panel_polished, nets_name_inside_panel_polished.end()));


            if(DEBUG_PANEL)std::cout << "nets name inside panel: " << nets_name_inside_panel.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_inside_intesected_panels: " << nets_inside_intesected_panels.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_name_inside_panel_polished: " << nets_name_inside_panel_polished.size() << std::endl;

            m_rtree_panel_box.insert(panel_box_current,panel_index);
            m_panel_index_to_nets_dict[panel_index] = nets_name_inside_panel_polished;

            // Wirelength of each panel
            std::vector<net_type> nets;
            for (auto net_name :nets_name_inside_panel_polished ){
                nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
            
            auto wl = m_design.global_routing().wirelength(nets);
            m_panel_wirelength_dict[panel_index] = wl;
            m_index_to_panel[panel_index] = panel_box_current;
            panel_ids.push_back(panel_index);
        }//end for 
        // m_panel_boxes.push_back(panels);
        m_panel_level[panel_level] = panel_ids;
        count++;
        // number_of_panels = 0;
    }//end while
    

   
}//end cluster_based_on_panel

void MCFMultiThreading::cluster_based_on_panel_v2(){
    auto chip_upper_right_corner = m_design.floorplan().chip_upper_right_corner();
    // The window is 10Gcellx width of layout
    int panel_base = 100;
    
    int layout_width = chip_upper_right_corner.x().value();
    int layout_height = chip_upper_right_corner.y().value();

    int panel_min_x = 1;
    int panel_min_y = 1;
    int panel_max_x = layout_width;
    int panel_max_y = panel_base;
    // Box2DWrapper panel_box(1,1,layout_width,panel_base);
    int number_of_panels = (layout_height/panel_base)+1;


    int count = 1;
    int total_panels = 0;
    unsigned int panel_level = 0;

    // horizontal 
    while(number_of_panels >= 1){
        if(panel_level==3) break;
        if(DEBUG_PANEL) std::cout << "level: " << count << std::endl;
        auto panel_size = panel_base * count;
        number_of_panels = ceil(layout_height/panel_size);
        // std::vector<Box2DWrapper> panels;
        panel_level++;
        std::vector<unsigned int> panel_ids;
        for (int panel_iterator = 0; panel_iterator <= number_of_panels ; panel_iterator++){
            // compute panel corner 
            panel_min_x = 0;//i*window_size;
            panel_min_y = panel_iterator*panel_size;
            panel_max_x = layout_width;
            panel_max_y = panel_min_y + panel_size;
            Box2DWrapper panel_box_current(panel_min_x,panel_min_y,panel_max_x,panel_max_y);
            unsigned int panel_index = total_panels;
            total_panels++;
            // panels.push_back(panel_box_current);

            if(DEBUG_PANEL) std::cout << "row_" << panel_iterator << ": " 
                          << panel_min_x << ", "
                          << panel_min_y << ", "
                          << panel_max_x << ", "
                          << panel_max_y << std::endl;
        

            auto nets_intersected_with_panel_pair = m_rtree_net_box.query(panel_box_current);
            
            std::vector<std::string> nets_name_inside_panel;
            std::set<std::string> nets_inside_intesected_panels;
            for (auto net_intersect_with_panel_pair : nets_intersected_with_panel_pair){
                auto net_inex = net_intersect_with_panel_pair.second;
                auto net_box  = net_intersect_with_panel_pair.first;
                auto net_name = m_index_net_box_to_net_name_dict[net_inex];
                Box2DWrapper net_box_wrapper(net_box);

                // if(DEBUG_MCF_MLT_NET_BOX_RTREE){
                //     std::cout << "value index: " << net_inex<< std::endl;
                //     std::cout << "net_name: " << net_name << std::endl;
                //     net_box_wrapper.report();     
                // }
                
                

                if((net_box_wrapper.getXl() >= panel_box_current.getXl()) &&
                   (net_box_wrapper.getYl() >= panel_box_current.getYl()) &&
                   (net_box_wrapper.getXh() <= panel_box_current.getXh()) &&
                   (net_box_wrapper.getYh() <= panel_box_current.getYh())
                ){
                    nets_name_inside_panel.push_back(net_name);
                }
            }//end for 

            Box2DWrapper panel_box_current_polish( panel_box_current.getXl(),
                                                    panel_box_current.getYl()+1,
                                                    panel_box_current.getXh(),
                                                    panel_box_current.getYh()-1);
            auto panels_intersect_with_current_panel_pair = m_rtree_panel_box.query(panel_box_current_polish);
            if(DEBUG_PANEL ) std::cout << "panels_intersect_with_current_panel_pair size: " << panels_intersect_with_current_panel_pair.size() << std::endl;
            for(auto panels_intersect_with_current_panel_pair : panels_intersect_with_current_panel_pair){
                auto nets_inside_intesected_panel = m_panel_index_to_nets_dict[panels_intersect_with_current_panel_pair.second];
                for(auto net_inside_intesected_panel: nets_inside_intesected_panel){
                    nets_inside_intesected_panels.insert(net_inside_intesected_panel);
                }//end for 
            }//end for 

                


            
            std::set<std::string> nets_name_inside_panel_polished;
            for(auto net : nets_name_inside_panel){
                if(nets_inside_intesected_panels.find(net) == nets_inside_intesected_panels.end())
                    nets_name_inside_panel_polished.insert(net);
            }
            // std::set_difference(nets_name_inside_panel.begin(), nets_name_inside_panel.end(),
            //                  nets_inside_intesected_panels.begin(), nets_inside_intesected_panels.end(),
            //                  std::inserter(nets_name_inside_panel_polished, nets_name_inside_panel_polished.end()));


            if(DEBUG_PANEL)std::cout << "nets name inside panel: " << nets_name_inside_panel.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_inside_intesected_panels: " << nets_inside_intesected_panels.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_name_inside_panel_polished: " << nets_name_inside_panel_polished.size() << std::endl;

            m_rtree_panel_box.insert(panel_box_current,panel_index);
            m_panel_index_to_nets_dict[panel_index] = nets_name_inside_panel_polished;

            // Wirelength of each panel
            std::vector<net_type> nets;
            for (auto net_name :nets_name_inside_panel_polished ){
                nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
            
            auto wl = m_design.global_routing().wirelength(nets);
            m_panel_wirelength_dict[panel_index] = wl;
            m_index_to_panel[panel_index] = panel_box_current;
            panel_ids.push_back(panel_index);
        }//end for 
        // m_panel_boxes.push_back(panels);
        m_panel_level[panel_level] = panel_ids;
        count++;
        // number_of_panels = 0;
    }//end while


    // Vertical Paneling
    number_of_panels = (layout_width/panel_base)+1;
    count = 1;
    while(number_of_panels >= 1){
        if(panel_level==6) break;
        if(DEBUG_PANEL) std::cout << "level: " << count << std::endl;
        auto panel_size = panel_base * count;
        number_of_panels = ceil(layout_height/panel_size);
        // std::vector<Box2DWrapper> panels;
        panel_level++;
        std::vector<unsigned int> panel_ids;
        for (int panel_iterator = 0; panel_iterator <= number_of_panels ; panel_iterator++){
            // compute panel corner 
            // vertical
            panel_min_x = panel_iterator*panel_size;
            panel_min_y = 0;//;
            panel_max_x = panel_min_x + panel_size;
            panel_max_y = layout_height;//;
            Box2DWrapper panel_box_current(panel_min_x,panel_min_y,panel_max_x,panel_max_y);
            unsigned int panel_index = total_panels;
            total_panels++;
            // panels.push_back(panel_box_current);

            if(DEBUG_PANEL) std::cout << "row_" << panel_iterator << ": " 
                          << panel_min_x << ", "
                          << panel_min_y << ", "
                          << panel_max_x << ", "
                          << panel_max_y << std::endl;
        

            auto nets_intersected_with_panel_pair = m_rtree_net_box.query(panel_box_current);
            
            std::vector<std::string> nets_name_inside_panel;
            std::set<std::string> nets_inside_intesected_panels;
            for (auto net_intersect_with_panel_pair : nets_intersected_with_panel_pair){
                auto net_inex = net_intersect_with_panel_pair.second;
                auto net_box  = net_intersect_with_panel_pair.first;
                auto net_name = m_index_net_box_to_net_name_dict[net_inex];
                Box2DWrapper net_box_wrapper(net_box);

                // if(DEBUG_MCF_MLT_NET_BOX_RTREE){
                //     std::cout << "value index: " << net_inex<< std::endl;
                //     std::cout << "net_name: " << net_name << std::endl;
                //     net_box_wrapper.report();     
                // }
                
                

                if((net_box_wrapper.getXl() >= panel_box_current.getXl()) &&
                   (net_box_wrapper.getYl() >= panel_box_current.getYl()) &&
                   (net_box_wrapper.getXh() <= panel_box_current.getXh()) &&
                   (net_box_wrapper.getYh() <= panel_box_current.getYh())
                ){
                    nets_name_inside_panel.push_back(net_name);
                }
            }//end for   

            Box2DWrapper panel_box_current_polish( 
                panel_box_current.getXl()+1,
                panel_box_current.getYl(),
                panel_box_current.getXh()-1,
                panel_box_current.getYh()
            );
            auto panels_intersect_with_current_panel_pair = m_rtree_panel_box.query(panel_box_current_polish);
            if(DEBUG_PANEL ) std::cout << "panels_intersect_with_current_panel_pair size: " << panels_intersect_with_current_panel_pair.size() << std::endl;
            for(auto panels_intersect_with_current_panel_pair : panels_intersect_with_current_panel_pair){
                auto nets_inside_intesected_panel = m_panel_index_to_nets_dict[panels_intersect_with_current_panel_pair.second];
                for(auto net_inside_intesected_panel: nets_inside_intesected_panel){
                    nets_inside_intesected_panels.insert(net_inside_intesected_panel);
                }//end for 
            }//end for 

                            
            std::set<std::string> nets_name_inside_panel_polished;
            for(auto net : nets_name_inside_panel){
                if(nets_inside_intesected_panels.find(net) == nets_inside_intesected_panels.end())
                    nets_name_inside_panel_polished.insert(net);
            }
            // std::set_difference(nets_name_inside_panel.begin(), nets_name_inside_panel.end(),
            //                  nets_inside_intesected_panels.begin(), nets_inside_intesected_panels.end(),
            //                  std::inserter(nets_name_inside_panel_polished, nets_name_inside_panel_polished.end()));


            if(DEBUG_PANEL)std::cout << "nets name inside panel: " << nets_name_inside_panel.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_inside_intesected_panels: " << nets_inside_intesected_panels.size() << std::endl;
            if(DEBUG_PANEL)std::cout << "nets_name_inside_panel_polished: " << nets_name_inside_panel_polished.size() << std::endl;

            m_rtree_panel_box.insert(panel_box_current,panel_index);
            m_panel_index_to_nets_dict[panel_index] = nets_name_inside_panel_polished;

            // Wirelength of each panel
            std::vector<net_type> nets;
            for (auto net_name :nets_name_inside_panel_polished ){
                nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
            
            auto wl = m_design.global_routing().wirelength(nets);
            m_panel_wirelength_dict[panel_index] = wl;
            m_index_to_panel[panel_index] = panel_box_current;
            panel_ids.push_back(panel_index);
        }//end for 
        // m_panel_boxes.push_back(panels);
        m_panel_level[panel_level] = panel_ids;
        count++;
        // number_of_panels = 0;
    }//end while
    

   
}//end cluster_based_on_panel_v2

void MCFMultiThreading::report(){
    auto panels_pair = m_rtree_panel_box.getValues();
    int total_nets = 0;
    for (auto panel_pair : panels_pair){
        auto panel_index = panel_pair.second;
        auto panel_box = panel_pair.first;
        Box2DWrapper panel_box_wrapper(panel_box);
        std::cout << panel_box_wrapper.getXl() << ", "<< panel_box_wrapper.getYl() << ", " << panel_box_wrapper.getXh() << ", " << panel_box_wrapper.getYh() << std::endl;
        auto nets = m_panel_index_to_nets_dict[panel_index];
        // for(auto net: nets){
        //     std::cout << net << " ";
        // }
        total_nets = total_nets + nets.size();
        std::cout << "num nets inside panel: " << nets.size() << std::endl;
    }//end for 
    std::cout << "total nets: " << total_nets << std::endl;

    // panel wl
    // for(auto panel : m_panel_wirelength_dict){
    //     auto panel_index = panel.first;
    //     auto nets = m_panel_index_to_nets_dict[panel_index];
    //     std::cout << "panel index: " << panel.first << ", wl: " << panel.second << ", num_nets: " << nets.size() << ", ";
    //     std::cout << "wl_ilp: " << m_panel_wirelength_after_ilp_dict[panel_index] << ", ";
        
        
    //     auto panel_box = m_index_to_panel[panel_index];
    //     std::cout << "box: " << panel_box.getXl() << ", " << panel_box.getYl() << ", " <<
    //                             panel_box.getXh() << ", " << panel_box.getYh();

    //     std::cout << std::endl;
    // }//end for 

    ofstream panel_info_file;
    std::string file_name = "panels_wl_" + std::to_string(num_nets_to_route) + ".txt";
    panel_info_file.open (file_name);
    panel_info_file << "panel_index,wl,num_nets,wl_ilp,xl,yl,xh,yh" << std::endl;
    for(auto panel : m_panel_wirelength_dict){
        auto panel_index = panel.first;
        auto panel_box = m_index_to_panel[panel_index];
        auto nets = m_panel_index_to_nets_dict[panel_index];
        panel_info_file << panel.first << "," 
               << panel.second << "," 
               << nets.size() << ", "
               << m_panel_wirelength_after_ilp_dict[panel_index] << ", "
               << panel_box.getXl() << ", " << panel_box.getYl() << ", " 
               << panel_box.getXh() << ", " << panel_box.getYh() << std::endl;

    }//end for 
    panel_info_file.close();

}//end report function

void MCFMultiThreading::run_ilp_on_panels(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    int num_panels = m_index_to_panel.size();
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    //std::vector<ophidian::circuit::CellInstance> cells(m_design.netlist().begin_cell_instance(), m_design.netlist().end_cell_instance());
    // ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
    // 
    for (unsigned iteration = 0; iteration < 1; iteration++) {
    for(int i = 0; i < num_panels; i++){    
        if(DEBUG_PANEL) std::cout << "panel_" << i << std::endl;
        auto nets_panel = m_panel_index_to_nets_dict[i];
        if(DEBUG_PANEL) std::cout << "num_nets" << nets_panel.size() << std::endl;
        
        auto panel_box = m_index_to_panel[i];
        auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl() + 1}, ophidian::util::database_unit_t{panel_box.getYl() + 1}};
        auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh() - 1}, ophidian::util::database_unit_t{panel_box.getYh() - 1}};
        auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};

        /*if (panel_max_corner.y().value() - panel_min_corner.y().value() > 100) {
            // not first level
            continue;
        }*/
        
        std::set<std::string> nets_name_set;
        std::vector<net_type> nets_local;
        int count_nets = 0;
        for(auto net : nets_panel){
            if(count_nets > num_nets_to_route){
                break;
            }else{
                nets_name_set.insert(net);
            }//end if
            count_nets++;
        }//end for 


        std::vector<ophidian::circuit::Net> fixed_nets;
        std::vector<ophidian::circuit::Net> routed_nets;
        std::vector<ophidian::circuit::Net> unrouted_nets;

        for(auto net: nets){
            std::string net_name = m_design.netlist().name(net);
            if(nets_name_set.find(net_name) == nets_name_set.end()){
                fixed_nets.push_back(net);
            }else{
                nets_local.push_back(m_net_name_to_net_type_dict[net_name]);
            }
            
        }

        auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
        m_design.placement().cells_within(panel_region, local_cells);

        if (DEBUG_PANEL) std::cout << "area " << panel_min_corner.x().value() << "," << panel_min_corner.y().value() << "," << panel_max_corner.x().value() << "," << panel_max_corner.y().value() << std::endl;
        if(DEBUG_PANEL) std::cout << "num_local_nets: " << nets_local.size() << std::endl;
        if (DEBUG_PANEL) std::cout << local_cells.size() << " local cells " << std::endl;

        /*auto debug_gcell = m_design.global_routing().gcell_graph()->gcell(90,82,6);
        auto capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
        auto demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);

        if (DEBUG_PANEL) std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/
        ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
        auto result = ilpRouting.route_nets(nets_local, local_cells, panel_region, fixed_nets, routed_nets, unrouted_nets, movements);

        m_design.placement().reset_rtree();
        
        /*capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
        demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
        if (DEBUG_PANEL) std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/

        if(DEBUG_PANEL) std::cout << "result ilproute: " << result.first << std::endl;
        if(DEBUG_PANEL) std::cout << "movements: " << movements.size() << std::endl;

        if (result.first == 0) {
            /*for (auto movement : movements) {
                auto cell = movement.first;
                auto cell_name = m_design.netlist().name(cell);
                std::cout << "moved cell " << cell_name << std::endl;
            }*/
            break;
        }

        std::vector<net_type> nets_set_update;

        for(auto net : routed_nets){
            nets_set_update.push_back(net);
            // m_fixed_nets.push_back(net);
        }

        for(auto net : unrouted_nets){
            nets_set_update.push_back(net);
            // m_fixed_nets.push_back(net);
        }

        if(DEBUG_PANEL) std::cout << "routed_nets size: " << routed_nets.size() << std::endl;
        if(DEBUG_PANEL) std::cout << "unrouted_nets size: " << unrouted_nets.size() << std::endl;
        if(DEBUG_PANEL) std::cout << "nets_set_update size: " << nets_set_update.size() << std::endl;

        auto wl_after_ilp = m_design.global_routing().wirelength(nets_set_update);
        if(DEBUG_PANEL) std::cout << "wl after ilp: " << wl_after_ilp << std::endl;
        
        m_panel_wirelength_after_ilp_dict[i] = wl_after_ilp;

        for(auto net_tmp : routed_nets){
            m_routed_nets.push_back(net_tmp);
        }


    }//end for 
        for (auto movement : movements) {
            auto cell = movement.first;
            m_design.placement().fixLocation(cell);
        }

    }

}//end run_ilp_on_panels
void MCFMultiThreading::reportPanelIds(){
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        auto ids = panel_level.second;
        std::cout << "level: " << level << "\n";
        for(auto id : ids){
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }
}//end reportPAnelIds

void MCFMultiThreading::run_ilp_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){  
    m_total_panel_nets = 0;
    auto number_of_levels = m_panel_level.size();
    if(DEBUG_PANEL_PARALLEL) std::cout << "num parallel levels: " << number_of_levels <<std::endl;
    std::vector<std::vector<unsigned int>> panels_vec;
    
    // for(auto panel_level: m_panel_level){
    //     std::vector<unsigned int> panel_indxs; 
    //     for(auto panel_idx : panel_level.second){
    //         panel_indxs.push_back(panel_idx);
    //     }
    //     panels_vec.push_back(panel_indxs);
    
    // }
    

    for(auto panel_level: m_panel_level){
    // for(int j = 5; j >= 1; j--){
        // auto panel_level = panels_vec[j-1];
        auto level = panel_level.first;
        // auto level = j;
        auto & ids = panel_level.second;
        // auto ids = panel_level;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        if(DEBUG_PANEL_PARALLEL) log() << "level: " << level << "\n";
        for(auto id : ids){
            if(id%2==0){
                even_ids.push_back(id);
            }else{
                odd_ids.push_back(id);
            }//end if
        }//end for 
        // omp_set_num_threads(8);
        // //even panels
        #pragma omp parallel for num_threads(8)
        for(int i = 0; i < even_ids.size(); i++){
            auto id = even_ids[i];
            // printf("Number of threads: %d",  omp_get_num_threads());
            //std::cout << "id: " << id << std::endl;
            std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> local_movements;
            run_ilp_on_panel(id,local_movements);

            #pragma omp critical
            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                movements.push_back(movement);
            } 

            //std::cout <<"even threads: " << omp_get_num_threads() << std::endl;
        }//end for
        update_global_routing();

        // odd panels
        #pragma omp parallel for num_threads(8)
        for(int i = 0; i < odd_ids.size(); i++){
            auto id = odd_ids[i];
            std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> local_movements;
            run_ilp_on_panel(id,local_movements);
            
            #pragma omp critical
            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                movements.push_back(movement);
            } 

            //std::cout <<"odd threads: " << omp_get_num_threads() << std::endl;
        }

        m_design.placement().reset_rtree();
        update_global_routing();
        // break;
        if(level == 5){
            break;
        }
        
    }//end for 

    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());

    std::cout << "total nets targeted by panel: " << m_total_panel_nets << "out of " << nets.size() << std::endl;
}//end run_ilp_on_panels_parallel


void MCFMultiThreading::run_ilp_on_panels_sequential(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){  
    m_total_panel_nets = 0;
    auto number_of_levels = m_panel_level.size();
    if(DEBUG_PANEL_PARALLEL) std::cout << "num parallel levels: " << number_of_levels <<std::endl;
    std::vector<std::vector<unsigned int>> panels_vec;
        

    for(auto panel_level: m_panel_level){;
        auto level = panel_level.first;
        auto & ids = panel_level.second;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        if(DEBUG_PANEL_PARALLEL) log() << "level: " << level << "\n";
        for(auto id : ids){
            if(id%2==0){
                even_ids.push_back(id);
            }else{
                odd_ids.push_back(id);
            }//end if
        }//end for 
        // omp_set_num_threads(8);

        // //even panels
        for(int i = 0; i < even_ids.size(); i++){
            auto id = even_ids[i];
            // printf("Number of threads: %d",  omp_get_num_threads());
            //std::cout << "id: " << id << std::endl;
            std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> local_movements;
            run_ilp_on_panel(id,local_movements);

            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                movements.push_back(movement);
            } 

            //std::cout <<"even threads: " << omp_get_num_threads() << std::endl;
        }//end for
        update_global_routing();

        // odd panels
        for(int i = 0; i < odd_ids.size(); i++){
            auto id = odd_ids[i];
            std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> local_movements;
            run_ilp_on_panel(id,local_movements);
            
            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                movements.push_back(movement);
            } 

            //std::cout <<"odd threads: " << omp_get_num_threads() << std::endl;
        }

        m_design.placement().reset_rtree();
        update_global_routing();
        // break;
        if(level == 5){
            break;
        }
        
    }//end for 

    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    std::cout << "total nets targeted by panel: " << m_total_panel_nets << "out of " << nets.size() << std::endl;
}//end run_ilp_on_panels_sequential

void MCFMultiThreading::run_ilp_on_panel(unsigned int panel_id,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    int num_panels = m_index_to_panel.size();
    
    //std::vector<ophidian::circuit::CellInstance> cells(m_design.netlist().begin_cell_instance(), m_design.netlist().end_cell_instance());
    // ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
    // 
    for (unsigned iteration = 0; iteration < 1; iteration++) {
        auto i = panel_id;
        if(DEBUG_PANEL) std::cout << "panel_" << i << std::endl;
        auto nets_panel = m_panel_index_to_nets_dict[i];
        if(DEBUG_PANEL) std::cout << "num_nets" << nets_panel.size() << std::endl;
        
        auto panel_box = m_index_to_panel[i];
        auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl() + 1}, ophidian::util::database_unit_t{panel_box.getYl() + 1}};
        auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh() - 1}, ophidian::util::database_unit_t{panel_box.getYh() - 1}};
        auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};

        /*if (panel_max_corner.y().value() - panel_min_corner.y().value() > 100) {
            // not first level
            continue;
        }*/
        
        std::set<std::string> nets_name_set;

        int count_nets = 0;
        for(auto net : nets_panel){
            if(count_nets > num_nets_to_route){
                break;
            }else{
                nets_name_set.insert(net);
            }//end if
            count_nets++;
        }//end for 

        m_total_panel_nets = m_total_panel_nets + nets_name_set.size();

        // if(nets_name_set.size() > num_nets_to_route/2){
        
            // int idx_range = i*int(nets_name_set.size()/2);
            // std::set<std::string> nets_name_set_first_half;
            // std::set<std::string> nets_name_set_second_half;
            // int counter = 0;
            // for(auto itr = nets_name_set.begin(); itr != nets_name_set.end();itr++){
            //     if(counter < nets_name_set.size()/2){
            //         nets_name_set_first_half.insert(*itr);
            //     }else{
            //         nets_name_set_second_half.insert(*itr);
            //     }
            //     counter++;
            // }//end for 
            
            // run_ilp(panel_region,nets_name_set_first_half,movements);
            // run_ilp(panel_region,nets_name_set_second_half,movements);
        
        // }else{
            run_ilp(panel_region,nets_name_set,movements);
        // }

        
        

       
        
        // m_ilp_results.push_back(ilp_result);

        // m_design.placement().reset_rtree();
        
        /*capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
        demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
        if (DEBUG_PANEL) std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/

        // if(DEBUG_PANEL) std::cout << "result ilproute: " << result.first << std::endl;
        // if(DEBUG_PANEL) std::cout << "movements: " << movements.size() << std::endl;

        // if (result.first == 0) {
        //     /*for (auto movement : movements) {
        //         auto cell = movement.first;
        //         auto cell_name = m_design.netlist().name(cell);
        //         std::cout << "moved cell " << cell_name << std::endl;
        //     }*/
        //     break;
        // }

        // std::vector<net_type> nets_set_update;

        // for(auto net : routed_nets){
        //     nets_set_update.push_back(net);
        //     // m_fixed_nets.push_back(net);
        // }

        // for(auto net : unrouted_nets){
        //     nets_set_update.push_back(net);
        //     // m_fixed_nets.push_back(net);
        // }

        // if(DEBUG_PANEL) std::cout << "routed_nets size: " << routed_nets.size() << std::endl;
        // if(DEBUG_PANEL) std::cout << "unrouted_nets size: " << unrouted_nets.size() << std::endl;
        // if(DEBUG_PANEL) std::cout << "nets_set_update size: " << nets_set_update.size() << std::endl;

        // auto wl_after_ilp = m_design.global_routing().wirelength(nets_set_update);
        // if(DEBUG_PANEL) std::cout << "wl after ilp: " << wl_after_ilp << std::endl;
        
        // m_panel_wirelength_after_ilp_dict[i] = wl_after_ilp;

        // for(auto net_tmp : routed_nets){
        //     m_routed_nets.push_back(net_tmp);
        // }


        
        // for (auto movement : movements) {
        //     auto cell = movement.first;
        //     m_design.placement().fixLocation(cell);
        // }

    }//end loop iteration
}//end run_ilp_on_panel

void MCFMultiThreading::run_astar_on_panels_parallel(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    auto number_of_levels = m_panel_level.size();
    if(DEBUG_PANEL_PARALLEL) std::cout << "num parallel levels: " << number_of_levels <<std::endl;
    std::vector<ophidian::circuit::Net> astar_nets;
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        if(level <= 5) continue;
        auto & ids = panel_level.second;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        log() << "level: " << level << "\n";
        for(auto & id : ids){
            if(id%2==0){
                even_ids.push_back(id);
            }else{
                odd_ids.push_back(id);
            }//end if
       
            auto & nets_panel = m_panel_index_to_nets_dict[id];
            for (auto net_name : nets_panel) {
                astar_nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
        }//end for 
         
        
        // //even panels
        //#pragma omp parallel for num_threads(8)
        for(int i = 0; i < even_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(even_ids[i]);
        }//end for 

        // // // odd panels
        //#pragma omp parallel for num_threads(8)
        for(int i = 0; i < odd_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(odd_ids[i]);
        }
    
        // if(level == 6)
        //     break;
    }//end for 

    // log() << "astar nets " << astar_nets.size() << std::endl;

    // /*auto debug_gcell = m_design.global_routing().gcell_graph()->gcell(34, 42, 2);
    // auto capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
    // auto demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
    // auto layer_index = m_design.global_routing().gcell_graph()->layer_index(debug_gcell);
    // auto gcell_box = m_design.global_routing().gcell_graph()->box(debug_gcell);

    // std::cout << "debug gcell " << gcell_box.min_corner().y().value() << " " << gcell_box.min_corner().x().value() << " " << layer_index << std::endl;
    // std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/    

    // ophidian::routing::AStarRouting astar_routing{m_design};
    // for(auto net : astar_nets){
    //     auto net_name = m_design.netlist().name(net);
    //     //log() << "astar on net " << net_name << " with pins " << m_design.netlist().pins(net).size() << std::endl;
    //     /*auto capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
    //     auto demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
    //     std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/
    //     if(DEBUG_ASTAR_PARALLEL) net_astar_file << net_name << ",";
    //     std::vector<ophidian::routing::AStarSegment> initial_segments;
    //     for(auto segment : m_design.global_routing().segments(net)) {
    //         initial_segments.push_back(ophidian::routing::AStarSegment(m_design.global_routing().box(segment), m_design.global_routing().layer_start(segment), m_design.global_routing().layer_end(segment), net));
    //     }
    //     m_design.global_routing().unroute(net);
    //     std::vector<ophidian::routing::AStarSegment> segments;
    //     auto result = astar_routing.route_net(net, segments, false);
    //     int init_wl = get_wire_length_segments(initial_segments);
    //     int astar_wl = get_wire_length_segments(segments);
    //     if(DEBUG_ASTAR_PARALLEL) net_astar_file << std::to_string(init_wl) << "," << std::to_string(astar_wl) << std::endl;
    //     if (result) {
    //         //log() << "applying segments" << std::endl;
    //         bool apply = astar_routing.apply_segments_to_global_routing(segments);
    //         if (!apply) {
    //             std::cout << "WARNING: FAILED TO APPLY" << std::endl;
    //         }
    //     } else {
    //         //log() << "undo segments" << std::endl;
    //         bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
    //         if(!undo) {
    //             std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
    //             std::cout << "NET " << net_name << std::endl;
    //             //break;
    //         }
    //     }            
    //     //log() << "done astar on net " << net_name << std::endl;
    // }

}//end run_astar_on_panels_parallel

void MCFMultiThreading::run_astar_on_panel(unsigned int panel_id){
    
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
     
     auto nets_panel = m_panel_index_to_nets_dict[panel_id];
     
        
    // 
    for(auto net_name : nets_panel){
        auto astar_result = run_astar_on_net(m_net_name_to_net_type_dict[net_name]);
        if(DEBUG_ASTAR_PARALLEL) net_astar_file  << net_name << ",";
        //#pragma omp critical
        update_astar_on_global_routing(astar_result);
   }//end for 
}//end run_astar_on_panel

void MCFMultiThreading::update_global_routing(){
    auto & global_routing = m_design.global_routing();
    for(auto m_ilp_result : m_ilp_results){
        auto routed_nets = m_ilp_result.m_routed_nets;
        for(auto routed_net : routed_nets){
            auto net = routed_net.first;
            auto ilp_segments = routed_net.second;
            global_routing.unroute(net);
            for(auto ilp_segment : ilp_segments){
                global_routing.add_segment(ilp_segment.wire_box, 
                            ilp_segment.start_layer, 
                            ilp_segment.stop_layer, net);
            }
            global_routing.increase_demand(net);

        }//end for 
        
        
    }//end for 
    m_ilp_results.clear();
}//end update_global_routing

void MCFMultiThreading::run_ilp(ophidian::placement::Placement::box_type panel_region,std::set<std::string> nets_name_set,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;
    std::vector<ophidian::circuit::Net> unrouted_nets;
    std::vector<net_type> nets_local;

    for(auto net: nets){
        std::string net_name = m_design.netlist().name(net);
        if(nets_name_set.find(net_name) == nets_name_set.end()){
            fixed_nets.push_back(net);
        }else{
            nets_local.push_back(m_net_name_to_net_type_dict[net_name]);
        }
        
    }

    auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
    m_design.placement().cells_within(panel_region, local_cells);

    // if (DEBUG_PANEL) std::cout << "area " << panel_min_corner.x().value() << "," << panel_min_corner.y().value() << "," << panel_max_corner.x().value() << "," << panel_max_corner.y().value() << std::endl;
    if(DEBUG_PANEL) std::cout << "num_local_nets: " << nets_local.size() << std::endl;
    //if (DEBUG_PANEL) std::cout << local_cells.size() << " local cells " << std::endl;

    /*auto debug_gcell = m_design.global_routing().gcell_graph()->gcell(90,82,6);
    auto capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
    auto demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
    
    if (DEBUG_PANEL) std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/

    
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
    // auto result = ilpRouting.route_nets(nets_local, local_cells, panel_region, fixed_nets, routed_nets, unrouted_nets, movements);
    auto ilp_result = ilpRouting.route_nets_v2(nets_local, local_cells, panel_region, fixed_nets, routed_nets, unrouted_nets, movements);
    // int s = ilp_result.m_routed_nets.size();
    #pragma omp critical
    m_ilp_results.push_back(ilp_result);
    


}//end run_ilp

void MCFMultiThreading::data_analysis(std::string file_name_data){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());

    // ofstream nets_info_file;
    // for(auto net : nets){
    //     auto net_name = m_design.netlist().name(net);
    // }//end for
    auto total_wl = m_design.global_routing().wirelength(nets);
    auto total_num_nets = nets.size();

    ofstream panel_info_file;
    std::string file_name = "panel_analysis_" + std::to_string(num_nets_to_route) + file_name_data + ".txt";
    panel_info_file.open (file_name);
    panel_info_file << "panel_level,panel_index,num_nets,total_nets,init_wl,total_wl,xl,yl,xh,yh" << std::endl;
    for(auto panel_level : m_panel_level){
        auto panel_indexs = m_panel_level[panel_level.first];
        for(auto panel_index : panel_indexs){
            auto nets_panel = m_panel_index_to_nets_dict[panel_index];
            auto panel_box = m_index_to_panel[panel_index];
            auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl() + 1}, ophidian::util::database_unit_t{panel_box.getYl() + 1}};
            auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh() - 1}, ophidian::util::database_unit_t{panel_box.getYh() - 1}};
            auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};
            auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
            m_design.placement().cells_within(panel_region, local_cells);
            
            std::vector<net_type> net_types;
            for(auto net_name : nets_panel){
                net_types.push_back(m_net_name_to_net_type_dict[net_name]);
            }
            auto wl = m_design.global_routing().wirelength(net_types);

            panel_info_file << panel_level.first << "," 
                            << panel_index << "," 
                            << nets_panel.size() << ", " << total_num_nets << ", " << wl << ", "
                            << total_wl << ", "
                            << panel_box.getXl() << ", " << panel_box.getYl() << ", " 
                            << panel_box.getXh() << ", " << panel_box.getYh() << std::endl;

        }//end for 
    }//end fpr 

    panel_info_file.close();


    ofstream panel_nets_info_file;
    file_name = "panel_nets_analysis_" + std::to_string(num_nets_to_route) + file_name_data + ".txt";
    panel_nets_info_file.open (file_name);
    panel_nets_info_file << "panel_index,nets" << std::endl;
    for(auto panel_level : m_panel_level){
        auto panel_indexs = m_panel_level[panel_level.first];
        for(auto panel_index : panel_indexs){
            auto nets_panel = m_panel_index_to_nets_dict[panel_index];
            panel_nets_info_file << panel_index << ", ";
            for(auto net : nets_panel){
                panel_nets_info_file << net << ", ";
            }
            panel_nets_info_file <<  "\n ";
        }//end for 
    }//end fpr 

    panel_nets_info_file.close();


     
    

    

}//end data_analysis

void MCFMultiThreading::run_astar_on_panels_parallel_v2(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    ophidian::routing::AStarRouting astar_routing{m_design};
    for(auto net : nets){
        auto net_name = m_design.netlist().name(net);
        /*auto capacity = m_design.global_routing().gcell_graph()->capacity(debug_gcell);
        auto demand = m_design.global_routing().gcell_graph()->demand(debug_gcell);
        std::cout << "debug gcell capacity " << capacity << " demand " << demand << std::endl;*/

        std::vector<ophidian::routing::AStarSegment> initial_segments;
        for(auto segment : m_design.global_routing().segments(net)) {
            initial_segments.push_back(ophidian::routing::AStarSegment(m_design.global_routing().box(segment), m_design.global_routing().layer_start(segment), m_design.global_routing().layer_end(segment), net));
        }
        m_design.global_routing().unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;
        auto result = astar_routing.route_net(net, segments, false);
        AstarResult astar_result;

        // if (result) {
        //     bool apply = astar_routing.apply_segments_to_global_routing(segments);
        //     if (!apply) {
        //         std::cout << "WARNING: FAILED TO APPLY" << std::endl;
        //     }
        // } else {
        //     bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        //     if(!undo) {
        //         std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
        //         std::cout << "NET " << net_name << std::endl;
        //         //break;
        //     }
        // }            
    }

}//end run_astar_on_panels_parallel_v2

AstarResult MCFMultiThreading::run_astar_on_net(net_type& net){
    
    
    auto net_name = m_design.netlist().name(net);
    std::vector<ophidian::routing::AStarSegment> initial_segments;
    for(auto segment : m_design.global_routing().segments(net)) {
        initial_segments.push_back(ophidian::routing::AStarSegment(m_design.global_routing().box(segment), m_design.global_routing().layer_start(segment), m_design.global_routing().layer_end(segment), net));
    }
        
    std::vector<ophidian::routing::AStarSegment> segments;
    bool result = false;
    #pragma omp critical
    {
        m_design.global_routing().unroute(net);
        result = m_astar_routing.route_net(net, segments, false);
    }
         
    AstarResult astar_result;
    astar_result.m_net = net;
    for(auto seg : initial_segments)
        astar_result.m_initial_segments.push_back(seg);
    for(auto seg : segments)
        astar_result.m_astar_segments.push_back(seg);
    astar_result.m_is_valid = result;
    
    return astar_result;
}//end run_astar_on_net

void MCFMultiThreading::update_astar_on_global_routing(AstarResult& astar_result){
    // ophidian::routing::AStarRouting astar_routing{m_design};
    auto net = astar_result.m_net;
    auto astar_segments = astar_result.m_astar_segments;
    auto initial_segments = astar_result.m_initial_segments;
    auto result = astar_result.m_is_valid;
    m_design.global_routing().unroute(net);
    auto net_name = m_design.netlist().name(net);

    int init_wl = get_wire_length_segments(initial_segments);
    int astar_wl = get_wire_length_segments(astar_segments);

    if(DEBUG_ASTAR_PARALLEL) net_astar_file  << std::to_string(init_wl) << "," << std::to_string(astar_wl) << std::endl;
    //  && (astar_wl < init_wl)
    if (result) {
        bool apply = m_astar_routing.apply_segments_to_global_routing(astar_segments);
        if (!apply) {
            std::cout << "WARNING: FAILED TO APPLY" << std::endl;
        }
    } else {
        bool undo = m_astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        if(!undo) {
            std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
            std::cout << "NET " << net_name << std::endl;
            //break;
        }
    }            
}//end apply_astar
bool MCFMultiThreading::cell_has_more_than_1_pin_in_same_net(ophidian::design::Design & design, ophidian::circuit::CellInstance cell)
{
    auto & netlist = design.netlist();
    std::unordered_map<std::string, int> pin_map;
    for(auto pin : netlist.pins(cell))
    {
        auto net = netlist.net(pin);
        auto net_name = netlist.name(net);
        pin_map[net_name]++;
    }
    for(auto map_it : pin_map)
        if(map_it.second > 1)
            return true;
    return false;
}

std::vector<std::pair<ophidian::circuit::CellInstance, double>> MCFMultiThreading::compute_cell_move_costs_descending_order(ophidian::design::Design & design)
{
    auto & netlist = design.netlist();
    auto & placement = design.placement();
    std::vector<std::pair<ophidian::circuit::CellInstance, double>> cells_costs;

    for(auto cell_it = netlist.begin_cell_instance(); cell_it != netlist.end_cell_instance(); ++cell_it)
    {
        auto cell = *cell_it;
        if(placement.isFixed(cell))
            continue;
        if(cell_has_more_than_1_pin_in_same_net(design, cell))
            continue;

        std::unordered_set<ophidian::circuit::Net, ophidian::entity_system::EntityBaseHash> cell_nets;
        for(auto pin : netlist.pins(cell))
        {
            auto net = netlist.net(pin);
            cell_nets.insert(net);
        }

        double routed_length = 0;
        double lower_bound = 0;
        for(auto net : cell_nets)
        {
            routed_length += design.global_routing().wirelength(net);
            //lower_bound += design.global_routing().lower_bound_wirelength(net);
        }
        auto cost = routed_length;
        //auto cost = routed_length / lower_bound;
        cells_costs.push_back(std::make_pair(cell, cost));
    }
    //SORT IN DESCENDING ORDER
    std::sort(cells_costs.begin(), cells_costs.end(), [](std::pair<ophidian::circuit::CellInstance, double> cost_a, std::pair<ophidian::circuit::CellInstance, double> cost_b) {return cost_a.second > cost_b.second;});
    return cells_costs;
}//end compute_cell_move_costs_descending_order






void MCFMultiThreading::calculate_median_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, std::vector<gcell_type> & target_gcells)
{
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;

    auto & netlist = design.netlist();
    auto & placement = design.placement();
    std::vector<net_type> cell_nets;
    
    std::vector<double> x_positions;
    std::vector<double> y_positions;

    for(auto pin : netlist.pins(cell)){
        auto net = netlist.net(pin);
        if(net == ophidian::circuit::Net())
            continue;
        cell_nets.push_back(net);
        auto x_min = std::numeric_limits<double>::max();
        auto y_min = std::numeric_limits<double>::max();
        auto x_max = -std::numeric_limits<double>::max();
        auto y_max = -std::numeric_limits<double>::max();
        for(auto net_pin : netlist.pins(net)){
            if(net_pin == pin)
                continue;
            auto location = placement.location(net_pin);
            x_min = std::min(x_min, location.x().value());
            y_min = std::min(y_min, location.y().value());
            x_max = std::max(x_max, location.x().value());
            y_max = std::max(y_max, location.y().value());
            //x_positions.push_back(location.x().value());
            //y_positions.push_back(location.y().value());
        }
        x_positions.push_back(x_min);
        x_positions.push_back(x_max);
        y_positions.push_back(y_min);
        y_positions.push_back(y_max);
    }
    auto cell_location = placement.location(cell);
    auto current_gcell = design.global_routing().gcell_graph()->nearest_gcell(cell_location, 0);

    if(x_positions.empty() || y_positions.empty()) {
        //return current_gcell;
        return;
    }

    std::nth_element(x_positions.begin(), x_positions.begin() + x_positions.size()/2, x_positions.end());
    auto median_x = x_positions[x_positions.size()/2];
    //auto median_x_left = x_positions[x_positions.size()/2];
    //auto median_x_right = x_positions[x_positions.size()/2 + 1];
    std::nth_element(y_positions.begin(), y_positions.begin() + y_positions.size()/2, y_positions.end());
    auto median_y = y_positions[y_positions.size()/2];
    //auto median_y_left = y_positions[y_positions.size()/2];
    //auto median_y_right = y_positions[y_positions.size()/2 + 1];

    //point_type median_point {unit_type(median_x), unit_type(median_y)};
    std::vector<point_type> median_points;
    median_points.push_back({unit_type{median_x}, unit_type{median_y}});
    /*median_points.push_back({unit_type{median_x+10}, unit_type{median_y}});
    median_points.push_back({unit_type{median_x-10}, unit_type{median_y}});
    median_points.push_back({unit_type{median_x}, unit_type{median_y+10}});
    median_points.push_back({unit_type{median_x}, unit_type{median_y-10}});*/
    /*median_points.push_back({unit_type{median_x_left}, unit_type{median_y_left}});
    median_points.push_back({unit_type{median_x_right}, unit_type{median_y_left}});
    median_points.push_back({unit_type{median_x_left}, unit_type{median_y_right}});
    median_points.push_back({unit_type{median_x_right}, unit_type{median_y_right}});*/

    for (auto median_point : median_points) {
        auto nearest_gcell = design.global_routing().gcell_graph()->nearest_gcell(median_point, 0);
        
        target_gcells.push_back(nearest_gcell);
    }

    /*if(current_gcell != nearest_gcell){
        return nearest_gcell;
    }
    return current_gcell;*/
}

MoveCellResult MCFMultiThreading::test_target_gcell(ophidian::design::Design & design, ophidian::circuit::CellInstance & cell, gcell_type & initial_gcell, gcell_type & target_gcell, ophidian::routing::AStarRouting & astar_routing) {
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    // using AStarSegment = ophidian::routing::AStarSegment;

    auto& global_routing = design.global_routing();
    auto& netlist = design.netlist();
    auto& placement = design.placement();
    auto& routing_constr = design.routing_constraints();
    auto& std_cells = design.standard_cells();
    auto gcell_graph_ptr = global_routing.gcell_graph();
    std::vector<AstarResult> astar_results;
    std::vector<AStarSegment> segments;
    std::vector<AStarSegment> astar_segments;

    // std::cout << "cell name: " << m_design.netlist().name(cell) << std::endl;
    // Get connected nets
    std::vector<net_type> cell_nets;
    for(auto pin : netlist.pins(cell))
    {
        auto net = netlist.net(pin);
        // std::cout << "net name: " << m_design.netlist().name(net) << std::endl;
        if(net == ophidian::circuit::Net())
            continue;
        cell_nets.push_back(net);
    }
    // std::cout << "cell_nets size: " << cell_nets.size() << std::endl;

    bool overflow_movement = false;
    //move cell to median
    #pragma omp critical
    overflow_movement = global_routing.move_cell(initial_gcell, target_gcell, cell, netlist, placement, routing_constr, std_cells);
    
    if(overflow_movement == false)
    {
        bool routed_all_nets = true;
        for(auto net : cell_nets)
        {
            // std::cout << "net: " << m_design.netlist().name(net) << std::endl;
            // #pragma omp critical
            // global_routing.unroute(net);
            #pragma omp critical
            routed_all_nets = m_astar_routing.route_net(net, segments, false);

            // for(auto seg: segments)
            //     astar_segments.push_back(seg);           

            // AstarResult astar_result = run_astar_on_net(net);
            if(routed_all_nets == false || overflow_movement == true){
                // routed_all_nets = false;
                break;
            }
        }//end for 
        if(routed_all_nets)
        {
            bool working_correct = true;
            // 
            
            // for(auto astar_result : astar_results){
            //     for(auto segments_astar : astar_result.m_astar_segments){
            //         segments.push_back(segments_astar);
            //     }//end for 
            // }//end for 
            // #pragma omp critical
            // working_correct = astar_routing.apply_segments_to_global_routing(segments);
            // for(auto astar_result : astar_results){
            //     #pragma omp critical
            //     update_astar_on_global_routing(astar_result);
            // }
            // auto wirelength_after = global_routing.wirelength(cell_nets);
            auto wirelength_after = get_wire_length_segments(segments);
            if(working_correct) {
                MoveCellResult move_cell_result;
                move_cell_result.m_cell_name = m_design.netlist().name(cell);
                for(auto net : cell_nets)
                    move_cell_result.m_nets.push_back(net);
                move_cell_result.m_astar_wl = wirelength_after;
                move_cell_result.m_is_valid = true;
                move_cell_result.m_init_gcell = initial_gcell;
                move_cell_result.m_target_gcell = target_gcell;
                for(auto seg : segments)
                    move_cell_result.m_astar_segments.push_back(seg);
                return move_cell_result;
                // return wirelength_after;
            }
        }
    }
    MoveCellResult move_cell_result;
    move_cell_result.m_is_valid = false;
    // return std::numeric_limits<double>::max();
    return move_cell_result;
}//end test_target_gcell

void MCFMultiThreading::run_compute_cell_move_costs_parallel(){
    auto number_of_levels = m_panel_level.size();
    if(DEBUG_PANEL_PARALLEL) std::cout << "num parallel levels: " << number_of_levels <<std::endl;
    std::vector<ophidian::circuit::Net> astar_nets;
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        //if(level <= 5) continue;
        auto & ids = panel_level.second;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        log() << "level: " << level << "\n";
        for(auto & id : ids){
            if(id%2==0){
                even_ids.push_back(id);
            }else{
                odd_ids.push_back(id);
            }//end if
       
            auto & nets_panel = m_panel_index_to_nets_dict[id];
            for (auto net_name : nets_panel) {
                astar_nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
        }//end for 
         
        
        // // //even panels
        // #pragma omp parallel for num_threads(8)
        for(int i = 0; i < even_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_compute_cell_move_costs_on_panel(even_ids[i]);
        }//end for 

        // // // odd panels
        // #pragma omp parallel for num_threads(8)
        for(int i = 0; i < odd_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_compute_cell_move_costs_on_panel(odd_ids[i]);
        }
    
        // if(level == 1)
        //     break;
    }//end for 
}//end run_compute_cell_move_costs_parallel

void MCFMultiThreading::run_compute_cell_move_costs_on_panel(unsigned int panel_id){
    int num_panels = m_index_to_panel.size();
    
    
    
    auto i = panel_id;
    if(DEBUG_PANEL) std::cout << "panel_" << i << std::endl;
    auto nets_panel = m_panel_index_to_nets_dict[i];
    if(DEBUG_PANEL) std::cout << "num_nets" << nets_panel.size() << std::endl;
        
    auto panel_box = m_index_to_panel[i];
    auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl() + 1}, ophidian::util::database_unit_t{panel_box.getYl() + 1}};
    auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh() - 1}, ophidian::util::database_unit_t{panel_box.getYh() - 1}};
    auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};

    
        
    std::set<std::string> cells_inside_panel;
    std::vector<net_type> local_nets;
    // std::vector<ophidian::circuit::CellInstance> local_cells;


    auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
    m_design.placement().cells_within(panel_region, local_cells);
    std::vector<net_type> cell_nets;

    for(auto cell : local_cells){
        
        auto cell_name = m_design.netlist().name(cell);
        if(m_cell_name_to_cell_type_dict.find(cell_name) == m_cell_name_to_cell_type_dict.end())
            m_cell_name_to_cell_type_dict[cell_name] = cell;

        std::cout << cell_name << ", ";


        for(auto pin : m_design.netlist().pins(cell))
        {
            
            auto net = m_design.netlist().net(pin);
            // std::cout << "net name: " << m_design.netlist().name(net) << std::endl;
            if(net == ophidian::circuit::Net())
                continue;
            cell_nets.push_back(net);
        }//end for 
        //cell_name to nets

                //cell_name to nets_box
        std::vector<int> location_xs;
        std::vector<int> location_ys;
        for(auto net : cell_nets){
            auto net_name = m_design.netlist().name(net);
            auto net_box = m_net_name_to_net_box_dict[net_name];
            int xl = net_box.getXl();
            int yl = net_box.getYl();
            int xh = net_box.getXh();
            int yh = net_box.getYh();
            location_xs.push_back(xl);
            location_xs.push_back(xh);
            location_ys.push_back(yl);
            location_ys.push_back(yh);
        }//end for 
        std::vector<int>::iterator min_x_itrerator = std::min_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator max_x_itrerator = std::max_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator min_y_itrerator = std::min_element(location_ys.begin(), location_ys.end());
        std::vector<int>::iterator max_y_itrerator = std::max_element(location_ys.begin(), location_ys.end());
        int min_x = *min_x_itrerator;
        int max_x = *max_x_itrerator;
        int min_y = *min_y_itrerator;
        int max_y = *max_y_itrerator;
        Box2DWrapper cell_net_box(min_x,min_y,max_x,max_y);

        std::cout << min_x << "," << min_y << ", " << max_x <<"," << max_y << ",";
        std::cout << panel_box.getXl() << "," << panel_box.getYl()
                  << ", " << panel_box.getXh() <<"," << panel_box.getYh() << std::endl;

        

        if( (cell_net_box.getXl() >= panel_box.getXl()) &&
            (cell_net_box.getYl() >= panel_box.getYl()) &&
            (cell_net_box.getXh() <= panel_box.getXh()) &&
            (cell_net_box.getYh() <= panel_box.getYh())
        ){
            cells_inside_panel.insert(cell_name);
        }


    }//end for 
   
    // auto cell_costs = compute_cell_move_costs_descending_order(m_design);

    m_panel_index_to_local_cells_dict[panel_id] = cells_inside_panel;
    std::cout << "panel id: " << panel_id << ", number of cells: " << cells_inside_panel.size() << std::endl;
 

}//end run_compute_cell_move_costs_on_panel

void MCFMultiThreading::move_cells_parallel(std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements){
    auto number_of_levels = m_panel_level.size();
    if(DEBUG_PANEL_PARALLEL) std::cout << "num parallel levels: " << number_of_levels <<std::endl;
    std::vector<ophidian::circuit::Net> astar_nets;
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        //if(level <= 5) continue;
        auto & ids = panel_level.second;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        log() << "level: " << level << "\n";
        for(auto & id : ids){
            if(id%2==0){
                even_ids.push_back(id);
            }else{
                odd_ids.push_back(id);
            }//end if
       
            auto & nets_panel = m_panel_index_to_nets_dict[id];
            for (auto net_name : nets_panel) {
                astar_nets.push_back(m_net_name_to_net_type_dict[net_name]);
            }
        }//end for 
         
        
        // // //even panels
        // #pragma omp parallel for num_threads(8)
        for(int i = 0; i < even_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_move_cell_on_panel(even_ids[i],movements);
        }//end for 

        // // // odd panels
        // #pragma omp parallel for num_threads(8)
        for(int i = 0; i < odd_ids.size(); i++){
            // std::cout << "id: " << id << std::endl;
            run_move_cell_on_panel(odd_ids[i],movements);
        }
    
        // if(level == 1)
        //     break;
    }//end for 

    
}//end move_cells_parallel

void MCFMultiThreading::run_move_cell_on_panel(unsigned int panel_id,std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements){
    auto cells_panel = m_panel_index_to_local_cells_dict[panel_id];
    
    auto& global_routing = m_design.global_routing();
    auto& netlist        = m_design.netlist();
    auto& placement      = m_design.placement();
    auto& routing_constr = m_design.routing_constraints();
    auto& std_cells      = m_design.standard_cells();
    auto gcell_graph_ptr = global_routing.gcell_graph();
    
    // move_cells_for_until_x_minutes(m_design, remaining_time, cell_costs, movements, m_astar_routing);
    std::cout << "panel id: " << panel_id << ", cell panel size: " << cells_panel.size() << std::endl;
    for(auto cell_name : cells_panel){
        auto cell = m_cell_name_to_cell_type_dict[cell_name];
        auto initial_location = m_design.placement().location(cell);
        auto initial_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(initial_location, 0);
        std::vector<gcell_type> target_gcells;
        //auto median_gcell = calculate_median_gcell(design, cell);
        calculate_median_gcell(m_design, cell, target_gcells);
        std::vector<net_type> cell_nets;
        // std::cout << "cell name: " << m_design.netlist().name(cell) << std::endl;
        for(auto pin : m_design.netlist().pins(cell))
        {
            
            auto net = m_design.netlist().net(pin);
            // std::cout << "net name: " << m_design.netlist().name(net) << std::endl;
            if(net == ophidian::circuit::Net())
                continue;
            cell_nets.push_back(net);
        }//end for 
        std::vector<AStarSegment> initial_segments;
        std::vector<AstarResult> initial_astar_results;
        for(auto net : cell_nets)
        {
            AstarResult astar_init_result;
            astar_init_result.m_net = net;
            for(auto segment : global_routing.segments(net)) {
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
                astar_init_result.m_initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
                astar_init_result.m_astar_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
            }
            astar_init_result.m_is_valid = true;
            initial_astar_results.push_back(astar_init_result);
        }
        auto best_gcell = gcell_type{};
        std::vector<AstarResult> astar_results;
        bool route_all_nets = true;
        for (auto target_gcell : target_gcells) {
            if (initial_gcell != target_gcell){
                auto overflow_movement = m_design.global_routing().move_cell(initial_gcell, target_gcell, cell, netlist, placement, routing_constr, std_cells);
                for(auto net : cell_nets){
                    auto astar_result = run_astar_on_net(net);
                    astar_results.push_back(astar_result);
                    if(!astar_result.m_is_valid)
                        route_all_nets = false;
                }
                for(auto astar_result : astar_results){
                    if(route_all_nets){
                        #pragma omp critical
                        update_astar_on_global_routing(astar_result);
                    }//end if 
                }//end for

                overflow_movement = m_design.global_routing().move_cell(target_gcell,initial_gcell, cell, netlist, placement, routing_constr, std_cells);
                for(auto astar_result : initial_astar_results)
                    update_astar_on_global_routing(astar_result);

                int init_wl = 0;
                int astar_wl = 0;
                for(auto astar_result : astar_results){
                    if(route_all_nets){
                        auto astar_segments = astar_result.m_astar_segments;
                        auto initial_segments = astar_result.m_initial_segments;
                        init_wl  = init_wl + get_wire_length_segments(initial_segments);
                        astar_wl = astar_wl + get_wire_length_segments(astar_segments);
                    }//end if 
                }//end for

                if(init_wl > astar_wl){
                    best_gcell = target_gcell;
                }
            }//end if 

 
        }//end for
        bool moved = false;
        if (best_gcell != gcell_type{}) {
            for (auto net : cell_nets) {
                global_routing.unroute(net);
            }
            astar_results.clear();
            auto overflow_movement = m_design.global_routing().move_cell(initial_gcell, best_gcell, cell, netlist, placement, routing_constr, std_cells);
            for(auto net : cell_nets){
                auto astar_result = run_astar_on_net(net);
                astar_results.push_back(astar_result);
                if(!astar_result.m_is_valid)
                    route_all_nets = false;
            }
            for(auto astar_result : astar_results){
                if(route_all_nets){
                    #pragma omp critical
                    update_astar_on_global_routing(astar_result);
                    moved = true;
                }//end if 
            }//end for
            
        } else {
            //std::cout << "could not find best gcell" << std::endl;
        }
        std::cout << "moved: " << moved << std::endl;
        #pragma omp critical
        if(moved)
        {
            auto location = m_design.placement().location(cell);
            movements.push_back(std::make_pair(cell, m_design.placement().location(cell)));
        }
     
        
   }//end for 

}//end run_move_cell_on_panel

void MCFMultiThreading::move_cells_parallel_v2(std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
                                    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements){
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    // using AStarSegment = ophidian::routing::AStarSegment;

    //cell_name to cell_type
    std::unordered_map<std::string,ophidian::circuit::CellInstance> cell_name_to_cell_type_dict;
    //cell_name to taget_gcells
    std::unordered_map<std::string,std::vector<gcell_type>> cell_name_to_target_gcells_dict;
    //cell_name to nets
    std::unordered_map<std::string,std::vector<net_type>> cell_name_to_nets_dict;
    
    //cell_name to min_wirelength
    std::unordered_map<std::string,int> cell_name_to_min_wire_length_dict;

    // cell name to id and vice versa
    std::unordered_map<std::string,int> cell_name_to_id_dict;
    std::unordered_map<int,std::string> id_to_cell_name_dict;

    //cell to cell_net_box
    std::unordered_map<std::string,Box2DWrapper> cell_name_to_net_box_dict;




    std::vector<ophidian::circuit::CellInstance> cells_vec;
    auto& global_routing = m_design.global_routing();
    auto& netlist        = m_design.netlist();
    auto& placement      = m_design.placement();
    auto& routing_constr = m_design.routing_constraints();
    auto& std_cells      = m_design.standard_cells();
    auto gcell_graph_ptr = global_routing.gcell_graph();

    int cell_name_id = 0;
    for(auto cell_pair : cells){
        auto cell_name = m_design.netlist().name(cell_pair.first);
        cell_name_to_cell_type_dict[cell_name] = cell_pair.first;
        cells_vec.push_back(cell_pair.first);
        cell_name_id++;
    }//end for 

    // #pragma omp parallel for num_threads(8)
    for(int i = 0; i < cells_vec.size(); i++){
        auto cell = cells_vec[i];
        
        if (m_design.placement().isFixed(cell)) {
            continue;
        }
        auto cell_name = m_design.netlist().name(cell);
        std::vector<gcell_type> target_gcells;
        //auto median_gcell = calculate_median_gcell(design, cell);
        calculate_median_gcell(m_design, cell, target_gcells);
        
        


        std::vector<net_type> cell_nets;
        // std::cout << "cell name: " << m_design.netlist().name(cell) << std::endl;
        for(auto pin : netlist.pins(cell))
        {
            
            auto net = netlist.net(pin);
            // std::cout << "net name: " << m_design.netlist().name(net) << std::endl;
            if(net == ophidian::circuit::Net())
                continue;
            cell_nets.push_back(net);
        }//end for 
        //cell_name to nets

                //cell_name to nets_box
        std::vector<int> location_xs;
        std::vector<int> location_ys;
        for(auto net : cell_nets){
            auto net_name = m_design.netlist().name(net);
            auto net_box = m_net_name_to_net_box_dict[net_name];
            int xl = net_box.getXl();
            int yl = net_box.getYl();
            int xh = net_box.getXh();
            int yh = net_box.getYh();
            location_xs.push_back(xl);
            location_xs.push_back(xh);
            location_ys.push_back(yl);
            location_ys.push_back(yh);
        }//end for 
        std::vector<int>::iterator min_x_itrerator = std::min_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator max_x_itrerator = std::max_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator min_y_itrerator = std::min_element(location_ys.begin(), location_ys.end());
        std::vector<int>::iterator max_y_itrerator = std::max_element(location_ys.begin(), location_ys.end());
        int min_x = *min_x_itrerator;
        int max_x = *max_x_itrerator;
        int min_y = *min_y_itrerator;
        int max_y = *max_y_itrerator;
        Box2DWrapper cell_net_box(min_x,min_y,max_x,max_y);
        cell_name_to_net_box_dict[cell_name] = cell_net_box;


        std::vector<AStarSegment> initial_segments;
        auto wirelength_before = global_routing.wirelength(cell_nets);
        for(auto net : cell_nets)
        {
            for(auto segment : global_routing.segments(net)) {
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
            }
        }//end for 
        auto min_wirelength = wirelength_before;
        //cell_name to min_wirelength
        // #pragma omp critical
        // {
            cell_name_to_target_gcells_dict[cell_name] = target_gcells;
            cell_name_to_min_wire_length_dict[cell_name] = (int)min_wirelength;
            cell_name_to_nets_dict[cell_name] = cell_nets;  
        // }
        
        // std::cout << "cell: " << cell_name << ", nets: ";
        // for(auto net : cell_nets){
        //     auto net_name = m_design.netlist().name(net);
        //     std::cout << net_name << ", ";
        // }
        // std::cout << std::endl;
    }//end for 


    // cell name levels
    // int level_size_base = 100; 
    // std::unordered_map<int, std::set<std::string>> level_to_cell_names_dict;
    // std::unordered_map<int, RTreeDB> level_to_rtree_dict;
    // //level to independent cell_boxes
    // std::unordered_map<int,std::vector<std::set<std::string>>> level_to_independent_cell_box_dict;

    // int level = 1;
    // std::set<std::string> cells_set;
    // // std::cout << "cells_vec size: " << cells_vec.size() << std::endl;
    // for(int i = 0 ; i < cells_vec.size(); i++){
    //     auto cell = cells_vec[i];
    //     auto cell_name = m_design.netlist().name(cell);

    //     auto cell_box = cell_name_to_net_box_dict[cell_name];
    //     if((abs(cell_box.getXh()- cell_box.getXl()) < level_size_base) &&
    //        (abs(cell_box.getYh()- cell_box.getYl()) < level_size_base)){
    //         cells_set.insert(cell_name);
    //     }//end if 
    // }//end for 

    // // std::cout << "cells_set size: " << cells_set.size() << std::endl;
    // level_to_cell_names_dict[level] = cells_set;

    // RTreeDB level_rtree;
    // cell_name_id = 0;
    // for(auto cell_name : cells_set){
    //     auto cell_box = cell_name_to_net_box_dict[cell_name];
    //     auto id = cell_name_to_id_dict[cell_name];
    //     cell_name_to_id_dict[cell_name] = cell_name_id;
    //     id_to_cell_name_dict[cell_name_id] = cell_name; 
    //     level_rtree.insert(cell_box);
    //     cell_name_id++;
    // }//end for 

    // level_rtree.report();

    // std::vector<std::set<std::string>> independet_cells_boxs;
    // std::set<std::string> selected_cells;
    // for(auto cell_name : cells_set){
    //     // std::cout << "cell name: " << cell_name << std::endl;
    //     if(selected_cells.find(cell_name) != selected_cells.end()) continue;
    //     std::set<std::string> cells_in_a_bunch;
    //     bool find_overlap_areas = true;
    //     cells_in_a_bunch.insert(cell_name);
    //     selected_cells.insert(cell_name);
        
    //     auto cell_box = cell_name_to_net_box_dict[cell_name];    
    //     auto ids_pair = level_rtree.query(cell_box);
    //     // std::cout << "ids pair: " << ids_pair.size() << std::endl;
    //     for(auto id_pair : ids_pair){
    //         auto id = id_pair.second;
    //         auto cell_name_neighbour = id_to_cell_name_dict[id];
    //         if(selected_cells.find(cell_name_neighbour) != selected_cells.end()) continue;
    //         cells_in_a_bunch.insert(cell_name_neighbour);
    //         selected_cells.insert(cell_name);
    //     }//end for
        
    //     independet_cells_boxs.push_back(cells_in_a_bunch);
        
    // }//end for 

    
    // std::cout << "independet_cells_boxs: " << independet_cells_boxs.size() << std::endl;



    // return;

    auto start_time = std::chrono::steady_clock::now();
    
    log() << "move cell and astar " << std::endl;
    bool moved = false;
    
    

    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < cells_vec.size(); i++){
            
            auto cell = cells_vec[i];           
            auto cell_name = m_design.netlist().name(cell);

            // std::cout << "cell_name: " << cell_name << std::endl;
            auto target_gcells = cell_name_to_target_gcells_dict[cell_name];
            std::vector<net_type> cell_nets;
            for(auto pin : netlist.pins(cell))
            {
                
                auto net = netlist.net(pin);
                // std::cout << "net name: " << m_design.netlist().name(net) << std::endl;
                if(net == ophidian::circuit::Net())
                    continue;
                cell_nets.push_back(net);
            }//end for 
            // auto cell_nets = cell_name_to_nets_dict[cell_name];
            auto initial_location = placement.location(cell);
            auto initial_gcell = gcell_graph_ptr->nearest_gcell(initial_location, 0);
            auto wirelength_before = global_routing.wirelength(cell_nets);
            auto min_wirelength = wirelength_before;

            std::vector<AStarSegment> initial_segments;
            for(auto net : cell_nets)
            {
                for(auto segment : global_routing.segments(net)) {
                    initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
                }
            }

            auto best_gcell = gcell_type{};
            for (auto target_gcell : target_gcells) {
                if (initial_gcell != target_gcell) {
                    bool overflow_movement = true;
                    
                    double wirelength = 0;
                    // #pragma omp critical
                    auto move_cell_result = test_target_gcell(m_design, cell, initial_gcell, target_gcell, m_astar_routing);
                    // #pragma omp critical
                    // overflow_movement = global_routing.move_cell(target_gcell,initial_gcell , cell, netlist, placement, routing_constr, std_cells);
                    if(move_cell_result.m_is_valid)
                        if (move_cell_result.m_astar_wl < min_wirelength) {
                            for(auto seg : initial_segments)
                                move_cell_result.m_initial_segments.push_back(seg);
                            #pragma omp critical
                            m_move_cells.push_back(move_cell_result);
                        }
                        // #pragma omp critical
                        // for(auto net : cell_nets) {
                        //     global_routing.unroute(net);
                        // }
                        // bool undo_overflow = true;
                        // #pragma omp critical
                        // undo_overflow = global_routing.move_cell(target_gcell, initial_gcell, cell, netlist, placement, routing_constr, std_cells);
                        // if(undo_overflow == true) {
                        //     std::cout<<"WARNING: UNDO MOVEMENT OVERFLOW!"<<std::endl;
                        // }
                        // bool undo = false;
                        // #pragma omp critical
                        // undo = m_astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
                        // if(undo == false) {
                        //     std::cout<<"WARNING: UNDO ROUTING OVERFLOW!"<<std::endl;
                        // }
                    
                }//end if 
            }//end for 
            // if (best_gcell != gcell_type{}) {
            //     // #pragma omp critical
            //     // for (auto net : cell_nets) {
            //     //     global_routing.unroute(net);
            //     // }
            //     bool result = false;
            //     // #pragma omp critical
            //     result = test_target_gcell(m_design, cell, initial_gcell, best_gcell, m_astar_routing);
            //     if (result == false) {
            //         std::cout << "WARNING: FAILED TO MOVE TO BEST GCELL" << std::endl;
            //     }
            //     moved = true;
            // } else {
            //     //std::cout << "could not find best gcell" << std::endl;
            //     moved = false;
            // }//end if
            // #pragma omp critical
            // if(moved)
            // {
            //     auto location = m_design.placement().location(cell);
            //     moved_cells++;            
            //     movements.push_back(std::make_pair(cell, m_design.placement().location(cell)));
            //     //std::cout << "moved to " << location.x().value() << "," << location.y().value() << std::endl;
            //     //std::cout<<"# of moved cells: "<<moved_cells<<std::endl;

            //     //auto wirelength = design.global_routing().wirelength(nets);
            //     //std::cout << "wirelength " << wirelength << std::endl;            
            // }

            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end_time-start_time;
            //std::cout << "time " << diff.count() << std::endl;
            bool time_out = false;//diff.count() > time_limit * 60.0 ? true : false;
            //if (cell_name == "C1245") break;
            //if(moved_cells == design.routing_constraints().max_cell_movement() || time_out)
            // if(moved_cells == m_design.routing_constraints().max_cell_movement() || time_out)
            // //if(moved_cells == 1)
            //     break;
        // }//end for 
        

    }//end for 



    // return;
    log() << "apply movements to global routing: " << m_move_cells.size() << std::endl;
    
    int moved_cells = 0;
    for(auto move_result : m_move_cells){
        auto cell_name = move_result.m_cell_name;
        auto initial_gcell = move_result.m_init_gcell;
        auto target_gcell = move_result.m_target_gcell;
        auto cell = cell_name_to_cell_type_dict[cell_name];
        auto init_segments = move_result.m_initial_segments;
        auto astar_segments = move_result.m_astar_segments;
        // std::cout << "cell name: " << cell_name << std::endl;
        for(auto net : move_result.m_nets){
            global_routing.unroute(net);
        }

        // auto location = m_design.placement().location(cell);
        // std::cout << "before move: " << location.x().value() << "," << location.y().value() << std::endl;
                   
        bool undo_overflow = global_routing.move_cell(initial_gcell, target_gcell, cell, netlist, placement, routing_constr, std_cells);

        // location = m_design.placement().location(cell);
        // std::cout << "after move: " << location.x().value() << "," << location.y().value() << std::endl;

        // std::cout << "undo_overflow: "<<  undo_overflow << std::endl ;

        bool bmoved = false;
        if(undo_overflow == true) {
            // std::cout<<"WARNING: MOVEMENT OVERFLOW!"<<std::endl;
            global_routing.move_cell(target_gcell,initial_gcell , cell, netlist, placement, routing_constr, std_cells);
            m_astar_routing.apply_segments_to_global_routing(init_segments);//This should never fail
        }else{
            bool undo = m_astar_routing.apply_segments_to_global_routing(astar_segments);//This should never fail
            // std::cout << "astar result: " << undo << std::endl;
            // for(auto seg : astar_segments){
            //     std::cout << "seg net: " << m_design.netlist().name(seg.net) << std::endl;
            // }
            if(undo == false) {
                // std::cout<<"WARNING: UNDO ROUTING OVERFLOW!"<<std::endl;
                for(auto net : move_result.m_nets){
                   global_routing.unroute(net);
                }
                global_routing.move_cell(target_gcell,initial_gcell , cell, netlist, placement, routing_constr, std_cells);
                m_astar_routing.apply_segments_to_global_routing(init_segments);//This should never fail
            }else{
                bmoved = true;
            }
        }
        
        if(bmoved)
        {
            auto location = m_design.placement().location(cell);
            moved_cells++;            
            movements.push_back(std::make_pair(cell, m_design.placement().location(cell)));
            //std::cout << "moved to " << location.x().value() << "," << location.y().value() << std::endl;
            //std::cout<<"# of moved cells: "<<moved_cells<<std::endl;

            //auto wirelength = design.global_routing().wirelength(nets);
            //std::cout << "wirelength " << wirelength << std::endl;
            if(moved_cells == m_design.routing_constraints().max_cell_movement())
                break;
            // break;
        }
    }//end for 

}//end move_cells_parallel_v2

void MCFMultiThreading::move_cells_parallel_v3(std::vector<std::pair<ophidian::circuit::CellInstance, double>> & cells,
    std::vector<std::pair<ophidian::circuit::CellInstance, ophidian::util::LocationDbu>> & movements){
    using unit_type = ophidian::util::database_unit_t;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;
    // using AStarSegment = ophidian::routing::AStarSegment;

    //cell_name to cell_type
    std::unordered_map<std::string,ophidian::circuit::CellInstance> cell_name_to_cell_type_dict;
    //cell_name to taget_gcells
    std::unordered_map<std::string,std::vector<gcell_type>> cell_name_to_target_gcells_dict;
    //cell_name to nets
    std::unordered_map<std::string,std::vector<net_type>> cell_name_to_nets_dict;
    

    //cell_name to min_wirelength
    std::unordered_map<std::string,int> cell_name_to_min_wire_length_dict;

    //cell to cell_net_box
    std::unordered_map<std::string,Box2DWrapper> cell_name_to_net_box_dict;



    std::vector<ophidian::circuit::CellInstance> cells_vec;
    auto& global_routing = m_design.global_routing();
    auto& netlist        = m_design.netlist();
    auto& placement      = m_design.placement();
    auto& routing_constr = m_design.routing_constraints();
    auto& std_cells      = m_design.standard_cells();
    auto gcell_graph_ptr = global_routing.gcell_graph();

    for(auto cell_pair : cells){
        auto cell_name = m_design.netlist().name(cell_pair.first);
        cell_name_to_cell_type_dict[cell_name] = cell_pair.first;
        cells_vec.push_back(cell_pair.first);
    }//end for 

    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < cells_vec.size(); i++){
        auto cell = cells_vec[i];
        
        if (m_design.placement().isFixed(cell)) {
            continue;
        }
        auto cell_name = m_design.netlist().name(cell);
        std::vector<gcell_type> target_gcells;
        //auto median_gcell = calculate_median_gcell(design, cell);
        calculate_median_gcell(m_design, cell, target_gcells);
        
        


        std::vector<net_type> cell_nets;
        for(auto pin : netlist.pins(cell))
        {
            auto net = netlist.net(pin);
            if(net == ophidian::circuit::Net())
                continue;
            cell_nets.push_back(net);
        }//end for 
        
        //cell_name to nets_box
        std::vector<int> location_xs;
        std::vector<int> location_ys;
        for(auto net : cell_nets){
            auto net_name = m_design.netlist().name(net);
            auto net_box = m_net_name_to_net_box_dict[net_name];
            int xl = net_box.getXl();
            int yl = net_box.getYl();
            int xh = net_box.getXh();
            int yh = net_box.getYh();
            location_xs.push_back(xl);
            location_xs.push_back(xh);
            location_ys.push_back(yl);
            location_ys.push_back(yh);
        }//end for 
        std::vector<int>::iterator min_x_itrerator = std::min_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator max_x_itrerator = std::max_element(location_xs.begin(), location_xs.end());
        std::vector<int>::iterator min_y_itrerator = std::min_element(location_ys.begin(), location_ys.end());
        std::vector<int>::iterator max_y_itrerator = std::max_element(location_ys.begin(), location_ys.end());
        int min_x = *min_x_itrerator;
        int max_x = *max_x_itrerator;
        int min_y = *min_y_itrerator;
        int max_y = *max_y_itrerator;
        Box2DWrapper cell_net_box(min_x,min_y,max_x,max_y);
        cell_name_to_net_box_dict[cell_name] = cell_net_box;



        std::vector<AStarSegment> initial_segments;
        auto wirelength_before = global_routing.wirelength(cell_nets);
        for(auto net : cell_nets)
        {
            for(auto segment : global_routing.segments(net)) {
                initial_segments.push_back(AStarSegment(global_routing.box(segment), global_routing.layer_start(segment), global_routing.layer_end(segment), net));
            }
        }//end for 
        auto min_wirelength = wirelength_before;
        //cell_name to min_wirelength
        #pragma omp critical
        {
            cell_name_to_target_gcells_dict[cell_name] = target_gcells;
            cell_name_to_min_wire_length_dict[cell_name] = (int)min_wirelength;
            cell_name_to_nets_dict[cell_name] = cell_nets;  
        }
        
        // std::cout << "cell: " << cell_name << ", nets: ";
        // for(auto net : cell_nets){
        //     auto net_name = m_design.netlist().name(net);
        //     std::cout << net_name << ", ";
        // }
        // std::cout << std::endl;
    }//end for 


    log() << "move cell and astar " << std::endl;
    #pragma omp parallel for num_threads(8)
    for(int i = 0; i < cells_vec.size(); i++){
        auto cell = cells_vec[i];
        auto cell_name = m_design.netlist().name(cell);
        auto target_gcells = cell_name_to_target_gcells_dict[cell_name];
        auto cell_nets = cell_name_to_nets_dict[cell_name];
        auto initial_location = placement.location(cell);
        auto initial_gcell = gcell_graph_ptr->nearest_gcell(initial_location, 0);
        auto best_gcell = gcell_type{};
        for (auto target_gcell : target_gcells) {
            if (initial_gcell != target_gcell) {
                auto overflow_movement = global_routing.move_cell(initial_gcell, target_gcell, cell, netlist, placement, routing_constr, std_cells);
                if(overflow_movement == false)
                {
                    std::vector<AStarSegment> segments;
                    bool routed_all_nets = true;
                    #pragma omp critical
                    for(int j = 0; j < cell_nets.size() ; j++)
                    {
                        auto net = cell_nets[j];
                        // #pragma omp critical
                        routed_all_nets = m_astar_routing.route_net(net, segments, false);
                        if(routed_all_nets == false)
                            break;
                    }
                    if(routed_all_nets)
                    {
                        //erfan
                        // bool working_correct = astar_routing.apply_segments_to_global_routing(segments);
                        bool working_correct = false;
                        auto wirelength_after = global_routing.wirelength(cell_nets);
                        // if(working_correct) {
                        //     return wirelength_after;
                        // }
                    }
                }
            }//end if 
        }//end for 

    }//end for 


 
    

}//end move_cells_parallel_v3

int MCFMultiThreading::get_wire_length_segments(const std::vector<AStarSegment> & segments){
 
    std::vector<ophidian::routing::GCell> gcells2;
    auto& routing_library = m_design.routing_library();
    for(auto segment : segments)
    {
        std::vector<ophidian::routing::GCell> gcells;
        auto box = segment.wire_box;
        auto layer_start = segment.start_layer; 
        auto layer_end = segment.end_layer;
        auto layer_start_index = routing_library.layerIndex(layer_start);
        auto layer_end_index = routing_library.layerIndex(layer_end);
        auto min_index = std::min(layer_start_index, layer_end_index);
        auto max_index = std::max(layer_start_index, layer_end_index);
        for(auto i = min_index; i <= max_index; i++)
        {
            auto box = segment.wire_box;
            m_design.global_routing().gcell_graph()->intersect(gcells, box, i-1);
        }
        for(auto gcell:gcells){
            gcells2.push_back(gcell);
        }
    }
    return gcells2.size();
}//end get_wire_length_segments

};//end namespace 

