#include "Engine.h"

#include <algorithm>
#include <set>
#include <iterator>
#include <fstream>
#include <iostream>
#include <chrono>
#include <random>

namespace UCal{

#define max_time_to_run 25

#define DEBUG_PANEL false

Engine::Engine(design_type & design):
    m_design(design)
{
    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // std::default_random_engine generator (seed);
    // std::uniform_real_distribution<double> distribution (0.0,1.0);
    // m_temperature = distribution(generator);
    // std::cout << "initial temperature: " << m_temperature << std::endl;
}//end Engine constructor

Engine::~Engine(){

}//end Engine destructor


void Engine::run(std::chrono::steady_clock::time_point start_time){
    log() << "A* routing only" << std::endl;
    

    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
  
    m_design.placement().reset_rtree();

    construct_net_boxes_rtree(nets);
    
    cluster_based_on_panel();

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end_time-start_time;
    auto current_time = diff.count() / 60.0;
    auto remaining_time = max_time_to_run - current_time;
    log() << "current time " << current_time << " remaining time " << remaining_time << std::endl;

    run_astar_on_panels_parallel(remaining_time);
}//end run 

// void Engine::run_date21(std::chrono::steady_clock::time_point start_time){
//     log() << "A* for generate the initial solution" << std::endl;
//     run_astar_on_circuit();
//     log() << "initial solution generated" << std::endl;

//     //ILP lower panels with movement 

//     //A* upper panels with movement

//     //A* all circuit with movements
// }

void Engine::run_astar_on_circuit(const std::vector<ophidian::circuit::Net> & nets){
    // std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    for(auto net : nets){
        auto astar_result = run_astar_on_net(net);
        if(astar_result.m_is_valid == false){
            std::cout << "A* FAIL IN NET " << m_design.netlist().name(net) << std::endl;
        }
        update_astar_on_global_routing(astar_result);
    }
}

void Engine::construct_net_boxes_rtree(const std::vector<net_type> &nets){
    auto & netlist = m_design.netlist();
    auto & placement = m_design.placement();    
    auto& global_routing = m_design.global_routing();
    auto& routing_library = m_design.routing_library();
        
    auto graph_ptr = global_routing.gcell_graph();

    //find net boxes
    unsigned int net_index = 1;
    for(auto net : nets){

 
            

        std::vector<int> location_xs;
        std::vector<int> location_ys;
        auto net_name = m_design.netlist().name(net);
 

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


void Engine::cluster_based_on_panel(){
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
                m_nets_inside_panels.insert(net_name);
            }
            
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
                m_nets_inside_panels.insert(net_name);
            }
            
            m_index_to_panel[panel_index] = panel_box_current;
            panel_ids.push_back(panel_index);
        }//end for 
        // m_panel_boxes.push_back(panels);
        m_panel_level[panel_level] = panel_ids;
        count++;
        // number_of_panels = 0;
    }//end while
    

   
}//end cluster_based_on_panel_v2


void Engine::run_astar_on_panels_parallel(int time_limit, int min_panel){
    auto start_time = std::chrono::steady_clock::now();
    bool time_out = false;
    auto number_of_levels = m_panel_level.size();
    std::vector<ophidian::circuit::Net> astar_nets;
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        if(level <= min_panel) continue;
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
         
        if(time_out) break;
        // //even panels
        #pragma omp parallel for num_threads(8)
        for(int i = 0; i < even_ids.size(); i++){
            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end_time-start_time;
            #pragma omp critical
            time_out = diff.count() > time_limit * 60.0 ? true : false;
            if(time_out){
                #pragma omp cancel for
            }
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(even_ids[i]);
        }//end for 

        if(time_out) break;

        // // // odd panels
        #pragma omp parallel for num_threads(8)
        for(int i = 0; i < odd_ids.size(); i++){
            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end_time-start_time;
            #pragma omp critical
            time_out = diff.count() > time_limit * 60.0 ? true : false;
            if(time_out){
                #pragma omp cancel for
            }
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(odd_ids[i]);
        }

        if(time_out) break;
    }//end for 

}//end run_astar_on_panels_parallel

void Engine::run_astar_on_panels_sequential(int time_limit, int min_panel){
    auto start_time = std::chrono::steady_clock::now();
    bool time_out = false;
    auto number_of_levels = m_panel_level.size();
    std::vector<ophidian::circuit::Net> astar_nets;
    for(auto panel_level: m_panel_level){
        auto level = panel_level.first;
        if(level <= min_panel) continue;
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
         
        if(time_out) break;
        // //even panels
        for(int i = 0; i < even_ids.size(); i++){
            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end_time-start_time;
            time_out = diff.count() > time_limit * 60.0 ? true : false;
            if(time_out){
                break;
            }
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(even_ids[i]);
        }//end for 

        if(time_out) break;

        // // // odd panels
       
        for(int i = 0; i < odd_ids.size(); i++){
            auto end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end_time-start_time;
           
            time_out = diff.count() > time_limit * 60.0 ? true : false;
            if(time_out){
                break;
            }
            // std::cout << "id: " << id << std::endl;
            run_astar_on_panel(odd_ids[i]);
        }

        if(time_out) break;
    }//end for 

}//end run_astar_on_panels_sequential

void Engine::run_astar_on_panel(unsigned int panel_id){
    
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
     
    auto nets_panel = m_panel_index_to_nets_dict[panel_id];
  
    
    for(auto net_name : nets_panel){
        auto astar_result = run_astar_on_net(m_net_name_to_net_type_dict[net_name]);
        #pragma omp critical
        update_astar_on_global_routing(astar_result);
   }//end for 
}//end run_astar_on_panel

AstarResultV2 Engine::run_astar_on_net(net_type& net){
    
    
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
         
    AstarResultV2 astar_result;
    astar_result.m_net = net;
    for(auto seg : initial_segments)
        astar_result.m_initial_segments.push_back(seg);
    for(auto seg : segments)
        astar_result.m_astar_segments.push_back(seg);
    astar_result.m_is_valid = result;
    
    return astar_result;
}//end run_astar_on_net

void Engine::update_astar_on_global_routing(AstarResultV2& astar_result){
    auto net = astar_result.m_net;
    auto astar_segments = astar_result.m_astar_segments;
    auto initial_segments = astar_result.m_initial_segments;
    auto result = astar_result.m_is_valid;
    m_design.global_routing().unroute(net);
    auto net_name = m_design.netlist().name(net);
    // std::cout << "init wl size: " << initial_segments.size() << std::endl;
    // std::cout << "astar_wl size: " << astar_segments.size() << std::endl;
    int init_wl  = get_wire_length_segments(initial_segments);
    int astar_wl = get_wire_length_segments(astar_segments);

    if(initial_segments.size() == 0){
        // case that not exist initial solution
        init_wl = std::numeric_limits<int>::max();
    }


    // std::cout << m_design.netlist().name(net) <<","<<std::to_string(init_wl) << "," << std::to_string(astar_wl) << std::endl;

    // int delta_cost = astar_wl - init_wl;

    // // if(delta_cost < 0){
    // //     result = result && true;
    // // }else{
    //     double coef = -1*delta_cost/m_temperature;
    //     unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    //     std::default_random_engine generator (seed);
    //     std::uniform_real_distribution<double> distribution (0.0,1.0);
    //     double r = distribution(generator);
    //     if(r < pow(10,coef)){
    //         result = result && true;
    //     }else{
    //         result = result && false;
    //     }


    //     // double alpha = distribution(generator);
    //     m_temperature = distribution(generator);
    //     std::cout << " temperature: " << m_temperature << std::endl;
    // // }
   
    if (result && (init_wl > astar_wl)) {
        bool apply = m_astar_routing.apply_segments_to_global_routing(astar_segments);
        if (!apply) {
            std::cout << "WARNING: FAILED TO APPLY" << std::endl;
        }
    } else if(initial_segments.size() > 0){
        bool undo = m_astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        if(!undo) {
            std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
            std::cout << "NET " << net_name << std::endl;
            //break;
        }
    }else{
        std::cout<<"WARNING: ROUTING FAILED, "<< m_design.netlist().name(net) << " are not routed!"<<std::endl;
    }
}//end apply_astar

int Engine::get_wire_length_segments(const std::vector<AStarSegment> & segments){
 
    std::vector<ophidian::routing::GCell> gcells;
    for(auto segment : segments)
    {
        auto layer_start_index = m_design.routing_library().layerIndex(segment.start_layer);
        auto layer_end_index = m_design.routing_library().layerIndex(segment.end_layer);
        auto min_index = std::min(layer_start_index, layer_end_index);
        auto max_index = std::max(layer_start_index, layer_end_index);
        for(auto i = min_index; i <= max_index; i++)
        {
            auto box = segment.wire_box;
            m_design.global_routing().gcell_graph()->intersect(gcells, box, i-1);
        }
    }
    //remove duplicated gcells
    std::sort(gcells.begin(), gcells.end(), [&](auto &lhs, auto &rhs){return m_design.global_routing().gcell_graph()->id(lhs) < m_design.global_routing().gcell_graph()->id(rhs);});
    gcells.erase(std::unique(gcells.begin(), gcells.end()), gcells.end());
    auto wirelength = gcells.size();
    return wirelength;
}//end get_wire_length_segments

};//end namespace 
