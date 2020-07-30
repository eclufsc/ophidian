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
bool DEBUG_PANEL = true;

#define num_nets_to_route 10000

MCFMultiThreading::MCFMultiThreading(design_type & design):
    m_design(design)
 //:
    // mDispatcher(std::shared_ptr<CDispatcher>(new CDispatcher(*this)))
{

}//end MCFMultiThreading constructor

MCFMultiThreading::~MCFMultiThreading(){
    // abortAndJoin();
}//end MCFMultiThreading destructor

void MCFMultiThreading::run(){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;

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

    cluster_based_on_panel();
    run_ilp_on_panels(movements);


    report();
    write_nets(movements);
    // if(DEBUG_MCF_MLT_NET_BOX_RTREE) m_rtree_net_box.report();
    
    


    // auto looper = std::make_unique<UCal::CLooper>();

    // std::cout << "Starting looper" << std::endl;
    // // To start and run
    // looper->run();

    // auto dispatcher = looper->getDispatcher();

    // std::cout << "Adding tasks" << std::endl;
    // for(uint32_t k=0; k<1; ++k)
    // {
    //     auto const task = [k]()
    //     { 
    //         std::cout << "Invocation " << k 
    //                 << ": Hello, I have been executed asynchronously on the looper for " << (k + 1) 
    //                 << " times." << std::endl;
    //         printf("Hellloooooo");
    //     };
    //     std::cout << "move tasks" << std::endl;
    //     dispatcher->post(std::move(task));
    // }

    // std::cout << "Waiting 5 seconds for completion" << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // std::cout << "Stopping looper" << std::endl;
    // // To stop it and clean it up
    // dispatcher = nullptr;
    // looper->stop();
    // looper = nullptr;
}//end run function


void MCFMultiThreading::construct_net_boxes_rtree(const std::vector<net_type> &nets){
    auto & netlist = m_design.netlist();
    auto & placement = m_design.placement();

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
        for(auto net_pin : netlist.pins(net)){

            auto location = placement.location(net_pin);               
            location_xs.push_back(location.x().value());
            location_ys.push_back(location.y().value());
            if(DEBUG_MCF_MLT_NET_BOX_RTREE)  std::cout << location.x().value() <<", " << location.y().value() << std::endl;
            //std::cout << "pin location " << location.x().value() << ", " << location.y().value() << std::endl;
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
        net_index++;
        
    }//end for 
}//end construct_net_boxes_rtree


void MCFMultiThreading::cluster_based_on_nets_box(){
    
    auto chip_upper_right_corner = m_design.floorplan().chip_upper_right_corner();
    // The window is 10Gcellx width of layout
    int window_size = 100;
    
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
    while(number_of_panels >= 1){
        if(DEBUG_PANEL) std::cout << "level: " << count << std::endl;
        auto panel_size = panel_base * count;
        number_of_panels = ceil(layout_height/panel_size);
        // std::vector<Box2DWrapper> panels;
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


            std::cout << "nets name inside panel: " << nets_name_inside_panel.size() << std::endl;
            std::cout << "nets_inside_intesected_panels: " << nets_inside_intesected_panels.size() << std::endl;
            std::cout << "nets_name_inside_panel_polished: " << nets_name_inside_panel_polished.size() << std::endl;

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

        }//end for 
        // m_panel_boxes.push_back(panels);
        count++;
        // number_of_panels = 0;
    }//end while
    

   
}//end cluster_based_on_panel

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
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
    // 
    for(int i = 0; i < num_panels; i++){    
        if(DEBUG_PANEL) std::cout << "panel_" << i << std::endl;
        auto nets_panel = m_panel_index_to_nets_dict[i];
        if(DEBUG_PANEL) std::cout << "num_nets" << nets_panel.size() << std::endl;
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

        auto panel_box = m_index_to_panel[i];
        auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl()}, ophidian::util::database_unit_t{panel_box.getYl()}};
        auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh()}, ophidian::util::database_unit_t{panel_box.getYh()}};
        auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};
        auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
        m_design.placement().cells_within(panel_region, local_cells);

        if(DEBUG_PANEL) std::cout << "num_local_nets: " << nets_local.size() << std::endl;
        if (DEBUG_PANEL) std::cout << local_cells.size() << " local cells " << std::endl;

        auto result = ilpRouting.route_nets(nets_local, local_cells, panel_region, fixed_nets, routed_nets, unrouted_nets, movements);

        if(DEBUG_PANEL) std::cout << "result ilproute: " << result.first << std::endl;
        if(DEBUG_PANEL) std::cout << "movements: " << movements.size() << std::endl;

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

}//end run_ilp_on_panels

};//end namespace 
