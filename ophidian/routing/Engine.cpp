#include "Engine.h"

#include <algorithm>
#include <set>
#include <iterator>
#include <fstream>
#include <iostream>
#include <random>
#include "TimerProfiler.h"
#include <cmath> 

namespace UCal{

#define max_time_to_run 100000

#define DEBUG_PANEL false
#define DEBUG_SECTION false
#define DEBUG_PANEL_PARALLEL false
#define num_nets_to_route 10000

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


Json Engine::run(movement_container_type & movements,std::chrono::steady_clock::time_point start_time){
    log() << "ILP with movements initing technique" << std::endl;
    // log() << "A* routing only" << std::endl;

    // loadParams();

    UCal::TimerProfiler timer_construct_net_boxes_rtree;
    UCal::TimerProfiler timer_cluster_based_on_panel;
    UCal::TimerProfiler timer_run_ilp_on_panels_parallel_loop;
    UCal::TimerProfiler timer_run_astar_on_panels_parallel_loop;
    UCal::TimerProfiler timer_run_astar_on_panels_parallel_section;
    UCal::TimerProfiler timer_run_astar_on_panels_sequential;

    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
  
    m_design.placement().reset_rtree();

    auto panel_start_time = std::chrono::steady_clock::now();

    // if(m_global_params["construct_net_boxes_rtree"] == "true"){
        log() << "construct_net_boxes_rtree\n";
        timer_construct_net_boxes_rtree.start();
        construct_net_boxes_rtree(nets);
        timer_construct_net_boxes_rtree.stop();
    // }//end if 
        

    // if(m_global_params["cluster_based_on_panel"] == "true"){
        log() << "cluster_based_on_panel\n";
        timer_cluster_based_on_panel.start();
        cluster_based_on_panel();
        timer_cluster_based_on_panel.stop();
    // }//end if 

    auto panel_end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_parser = panel_end_time-panel_start_time;
    log() << "TIME CREATE PANELS : " << time_parser.count() << std::endl;

    // auto end_time = std::chrono::steady_clock::now();
    // std::chrono::duration<double> diff = end_time-start_time;
    // auto current_time = diff.count() / 60.0;
    // auto remaining_time = max_time_to_run - current_time;
    // log() << "current time " << current_time << " remaining time " << remaining_time << std::endl;

    // if(m_global_params["run_ilp_on_panels_parallel"] == "true"){
        timer_run_ilp_on_panels_parallel_loop.start();
        m_design.global_routing().set_gcell_cell_instances(m_design.netlist(), m_design.placement());
        log() << "ilp parallel\n";
        run_ilp_on_panels_parallel(movements);
        log() << "end ilp parallel\n";
        timer_run_ilp_on_panels_parallel_loop.stop();
    // }
    
    // if(m_global_params["run_astar_on_panels_parallel_loop"] == "true"){
    //     std::cout << "loop\n";
    //     timer_run_astar_on_panels_parallel_loop.start();
    //     run_astar_on_panels_parallel_loop(remaining_time);
    //     timer_run_astar_on_panels_parallel_loop.stop();
    // }
    // if(m_global_params["run_astar_on_panels_parallel_section"] == "true"){
    //     std::cout << "section\n";
    //     timer_run_astar_on_panels_parallel_section.start();
    //     run_astar_on_panels_parallel_section(remaining_time);
    //     timer_run_astar_on_panels_parallel_section.stop();
    // }
    // if(m_global_params["run_astar_on_panels_sequential"] == "true"){
    //     std::cout << "sequential\n";
    //     timer_run_astar_on_panels_sequential.start();
    //     run_astar_on_panels_sequential(remaining_time);
    //     timer_run_astar_on_panels_sequential.stop();
    // }

    m_timer_report_json["construct_net_boxes_rtree"] = timer_construct_net_boxes_rtree.userTime();
    m_timer_report_json["cluster_based_on_panel"] = timer_cluster_based_on_panel.userTime();
    m_timer_report_json["run_ilp_on_panels_parallel_loop"] = timer_run_ilp_on_panels_parallel_loop.userTime();
    m_timer_report_json["run_astar_on_panels_parallel_loop"] = timer_run_astar_on_panels_parallel_loop.userTime();
    m_timer_report_json["run_astar_on_panels_parallel_section"] = timer_run_astar_on_panels_parallel_section.userTime();
    m_timer_report_json["run_astar_on_panels_sequential"] = timer_run_astar_on_panels_sequential.userTime();
    
    log() << "ILP with movements end technique" << std::endl;
    return m_timer_report_json;
    // // write prettified JSON to another file
    // std::string jsonFileNamestr = m_timer_report_json["file_name_report"];
    // std::ifstream jsonFile(jsonFileNamestr, std::ifstream::binary);
    // std::ofstream write_report_file(jsonFileNamestr);
    // write_report_file << std::setw(4) << m_timer_report_json << std::endl;   

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
    auto timer = ophidian::util::timer();

    // std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    unsigned count = 0;
    unsigned total_count = nets.size();
    for(auto net : nets){
        auto net_name = m_design.netlist().name(net);

        timer.start();
        auto astar_result = run_astar_on_net(net);
        // auto runtime = timer.elapsed();
        // std::cout << "net " << net_name << " runtime " << runtime << std::endl;
        // log() << count << " out of " << total_count << std::endl;
        count++;
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
    auto & floorplan = m_design.floorplan();
    auto chip_origin = floorplan.chip_origin();
    auto chip_upper_right_corner = floorplan.chip_upper_right_corner();

    // The window is 10Gcellx width of layout
    auto row = *floorplan.range_row().begin();
    auto row_height = floorplan.upper_right_corner(row).y().value() - floorplan.origin(row).y().value();
    // int panel_base = 100;
    int panel_base = 10*row_height;
    
    int layout_width = chip_upper_right_corner.x().value() - chip_origin.x().value();
    int layout_height = chip_upper_right_corner.y().value() - chip_origin.y().value();

    int panel_min_x = 1;
    int panel_min_y = 1;
    int panel_max_x = layout_width;
    int panel_max_y = panel_base;
    // Box2DWrapper panel_box(1,1,layout_width,panel_base);
    int number_of_panels = (layout_height/panel_base)+1;


    int count = 1;
    int total_panels = 0;
    unsigned int panel_level = 0;

    int levels = std::ceil(log2(number_of_panels)) +1;

    // horizontal 
    while(number_of_panels >= 1){
        if(panel_level==levels) break;
        if(DEBUG_PANEL) std::cout << "level: " << count << std::endl;
        auto panel_size = panel_base * count;
        number_of_panels = ceil(layout_height/panel_size);
        // std::vector<Box2DWrapper> panels;
        panel_level++;
        std::vector<unsigned int> panel_ids;
        for (int panel_iterator = 0; panel_iterator <= number_of_panels ; panel_iterator++){
            // compute panel corner 
            // panel_min_x = 0;//i*window_size;
            // panel_min_y = panel_iterator*panel_size;
            // panel_max_x = layout_width;
            // panel_max_y = panel_min_y + panel_size;

            panel_min_x = chip_origin.x().value();
            panel_min_y = panel_iterator*panel_size + chip_origin.y().value();
            panel_max_x = layout_width + chip_origin.x().value();;
            panel_max_y = panel_min_y + panel_size + chip_origin.y().value();


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
        if(panel_level==levels) break;
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


                        // compute panel corner 
            // panel_min_x = 0;//i*window_size;
            // panel_min_y = panel_iterator*panel_size;
            // panel_max_x = layout_width;
            // panel_max_y = panel_min_y + panel_size;

            panel_min_x = chip_origin.x().value();
            panel_min_y = panel_iterator*panel_size + chip_origin.y().value();
            panel_max_x = layout_width + chip_origin.x().value();;
            panel_max_y = panel_min_y + panel_size + chip_origin.y().value();



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
void Engine::run_astar_on_panels_sequential(int time_limit){
    auto start_time = std::chrono::steady_clock::now();
    bool time_out = false;
    auto number_of_levels = m_panel_level.size();
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
         
      
        for(int i = 0; i < even_ids.size(); i++){
            run_astar_on_panel(even_ids[i]);
        }//end for 


        for(int i = 0; i < odd_ids.size(); i++){
            run_astar_on_panel(odd_ids[i]);
        }

       
    }//end for 

}//end run_astar_on_panels_parallel

void Engine::run_astar_on_panels_parallel_loop(int time_limit){
    auto start_time = std::chrono::steady_clock::now();
    bool time_out = false;
    auto number_of_levels = m_panel_level.size();
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


void Engine::run_astar_on_panels_parallel_section(int time_limit){
    auto start_time = std::chrono::steady_clock::now();
    bool time_out = false;
    auto number_of_levels = m_panel_level.size();
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
         
        if(time_out) break;
        // //even panels
        omp_set_num_threads( 8 );
        if(DEBUG_SECTION) std::cout << "even_ids.size(): " << even_ids.size() << std::endl;
        if(DEBUG_SECTION) std::cout << "odd_ids.size(): " << odd_ids.size() << std::endl;
        if(DEBUG_SECTION) std::cout << "omp_get_num_threads: "<< omp_get_num_threads() <<std::endl;

        
        for(int i = 0; i < even_ids.size(); i+=8)
        {
            #pragma omp parallel sections
            {
                // Number 1
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section0_even\n";
                    if(i <= even_ids.size())
                        run_astar_on_panel(even_ids[i]);
                }

                // Number 2
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section1_even\n";
                    if(i+1 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+1]);
                }

                // Number 3
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section2_even\n";
                    if(i+2 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+2]);
                }

                // Number 4
                #pragma omp section
                {
                    if(DEBUG_SECTION)  std::cout << "section3_even\n";
                    if(i+3 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+3]);
                }

                // Number 5
                #pragma omp section
                {
                    if(DEBUG_SECTION)  std::cout << "section4_even\n";
                    if(i+4 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+4]);
                }

                // Number 6
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section5_even\n";
                    if(i+5 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+5]);
                }

                // Number 7
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section6_even\n";
                    if(i+6 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+6]);
                }

                // Number 8
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section7_even\n";
                    if(i+7 <= even_ids.size())
                        run_astar_on_panel(even_ids[i+7]);
                }
                    
            }
        }//end for 


        
        for(int i = 0; i < odd_ids.size(); i+=8)
        {
            #pragma omp parallel sections
            {
                // Number 1
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section0_odd\n";
                    if(i <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i]);
                }

                // Number 2
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section1_odd\n";
                    if(i+1 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+1]);
                }

                // Number 3
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section2_odd\n";
                    if(i+2 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+2]);
                }

                // Number 4
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section3_odd\n";
                    if(i+3 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+3]);
                }

                // Number 5
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section4_odd\n";
                    if(i+4 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+4]);
                }

                // Number 6
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section5_odd\n";
                    if(i+5 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+5]);
                }

                // Number 7
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section6_odd\n";
                    if(i+6 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+6]);
                }

                // Number 8
                #pragma omp section
                {
                    if(DEBUG_SECTION) std::cout << "section7_odd\n";
                    if(i+7 <= odd_ids.size())
                        run_astar_on_panel(odd_ids[i+7]);
                }
                    
            }
        }//end for 


        // // // odd panels
        // #pragma omp parallel sections
        // for(int i = 0; i < odd_ids.size(); i++){
        //     auto end_time = std::chrono::steady_clock::now();
        //     std::chrono::duration<double> diff = end_time-start_time;
        //     #pragma omp critical
        //     time_out = diff.count() > time_limit * 60.0 ? true : false;
        //     if(time_out){
        //         #pragma omp cancel for
        //     }
        //     // std::cout << "id: " << id << std::endl;
        //     run_astar_on_panel(odd_ids[i]);
        // }

        // if(time_out) break;
    }//end for 

}//end run_astar_on_panels_parallel

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

    //log() << "init wl " << init_wl << " astar wl " << astar_wl << " result " << result << std::endl;

    // if(initial_segments.size() == 0){
    //     // case that not exist initial solution
    //     init_wl = std::numeric_limits<int>::max();
    // }


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
   
    if (result && (init_wl == 0 || init_wl > astar_wl)) {
        bool apply = m_astar_routing.apply_segments_to_global_routing(astar_segments);
        if (!apply) {
            std::cout << "WARNING: FAILED TO APPLY" << std::endl;
        }
    // } else if(initial_segments.size() > 0){
    } else{
        bool undo = m_astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
        if(!undo) {
            std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
            std::cout << "NET " << net_name << std::endl;
            //break;
        }
    }
    // else{
    //     std::cout<<"WARNING: ROUTING FAILED, "<< m_design.netlist().name(net) << " are not routed!"<<std::endl;
    // }
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


void Engine::loadParams(){
    
    std::ifstream jsonFileName("file_name.json", std::ifstream::binary);
    Json tmpFileName; 
    jsonFileName >> tmpFileName;
    std::string jsonFileNamestr = tmpFileName["file_name"];
    m_timer_report_json["file_name_report"] = tmpFileName["file_name_report"];
    std::ifstream jsonFile(jsonFileNamestr, std::ifstream::binary);
    jsonFile >> m_global_params;

    // std::cout << "construct_net_boxes_rtree"           << m_global_params["construct_net_boxes_rtree"           ]          << std::endl;
    // std::cout << "cluster_based_on_panel"              << m_global_params["cluster_based_on_panel"              ]              << std::endl;
    // std::cout << "run_astar_on_panels_parallel_loop"   << m_global_params["run_astar_on_panels_parallel_loop"   ]    << std::endl;
    // std::cout << "run_astar_on_panels_parallel_section"<< m_global_params["run_astar_on_panels_parallel_section"] << std::endl;
    // std::cout << "run_astar_on_panels_sequential"      << m_global_params["run_astar_on_panels_sequential"      ]       << std::endl;


}//end method 

void Engine::run_ilp_on_panels_parallel(movement_container_type & movements){  
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
        #pragma omp parallel for num_threads(8) //ilsvlsi
        for(int i = 0; i < even_ids.size(); i++){
            auto id = even_ids[i];
            // printf("Number of threads: %d",  omp_get_num_threads());
            //std::cout << "id: " << id << std::endl;
            movement_container_type local_movements;
            run_ilp_on_panel(id,local_movements);

            #pragma omp critical //ilsvlsi
            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                // movements.push_back(movement);
                movements[movement.first] = movement.second;
            } 

            //std::cout <<"even threads: " << omp_get_num_threads() << std::endl;
        }//end for
        update_global_routing();

        // odd panels
        #pragma omp parallel for num_threads(8) //ilsvlsi
        for(int i = 0; i < odd_ids.size(); i++){
            auto id = odd_ids[i];
            movement_container_type local_movements;
            run_ilp_on_panel(id,local_movements);
            
            #pragma omp critical //ilsvlsi
            for (auto movement : local_movements) {
                auto source_location = m_design.placement().location(movement.first);
                auto source_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(source_location, 0);
                //m_design.placement().place(movement.first, movement.second);
                auto target_gcell = m_design.global_routing().gcell_graph()->nearest_gcell(movement.second, 0);
                m_design.global_routing().move_cell(source_gcell, target_gcell, movement.first, m_design.netlist(), m_design.placement(), m_design.routing_constraints(), m_design.standard_cells());
                //m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
                // movements.push_back(movement);
                movements[movement.first] = movement.second;
            } 

            //std::cout <<"odd threads: " << omp_get_num_threads() << std::endl;
        }

        m_design.placement().reset_rtree();
        update_global_routing();
        // break;
        // if(level == 5){
        //     break;
        // }
        
    }//end for 

    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());

    // std::cout << "total nets targeted by panel: " << m_total_panel_nets << "out of " << nets.size() << std::endl;
}//end run_ilp_on_panels_parallel


void Engine::update_global_routing(){
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

void Engine::run_ilp_on_panel(unsigned int panel_id,movement_container_type & movements){
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


void Engine::run_ilp(ophidian::placement::Placement::box_type panel_region,std::set<std::string> nets_name_set,movement_container_type & movements){
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
    
    #pragma omp critical //ILSVLSI
    m_ilp_results.push_back(ilp_result);
    


}//end run_ilp

};//end namespace 
