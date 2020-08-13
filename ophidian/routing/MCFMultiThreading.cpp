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
bool DEBUG_PANEL_PARALLEL = true;

#define num_nets_to_route 10000
#define WINDOW_SIZE 100 // means each panels covers 10 gcells in y-axis

MCFMultiThreading::MCFMultiThreading(design_type & design):
    m_design(design)
 //:
    // mDispatcher(std::shared_ptr<CDispatcher>(new CDispatcher(*this)))
{

}//end MCFMultiThreading constructor

MCFMultiThreading::~MCFMultiThreading(){
    // abortAndJoin();
}//end MCFMultiThreading destructor

void MCFMultiThreading::run(std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> & movements){
    std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
    //std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;

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
    //data_analysis("before");
    // run_ilp_on_panels(movements);
    // run_ilp_on_panel(1,movements);

    m_design.global_routing().set_gcell_cell_instances(m_design.netlist(), m_design.placement());
    run_ilp_on_panels_parallel(movements);
    run_astar_on_panels_parallel(movements);
    //data_analysis("after");

    
    //write_nets(movements);
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
        auto ids = panel_level.second;
        // auto ids = panel_level;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        if(DEBUG_PANEL_PARALLEL) std::cout << "level: " << level << "\n";
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
                m_design.placement().place(movement.first, movement.second);
                m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
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
                m_design.placement().place(movement.first, movement.second);
                m_design.global_routing().update_blockage_demand(m_design.netlist(), m_design.placement(), movement.first, false);
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
        auto ids = panel_level.second;
        if(level <= 5) continue;
        std::vector<unsigned int> even_ids;
        std::vector<unsigned int> odd_ids;
        std::cout << "level: " << level << "\n";
        for(auto id : ids){
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
        // // #pragma omp parallel for num_threads(8)
        /*for(auto id : even_ids){
            std::cout << "id: " << id << std::endl;
            run_astar_on_panel(id);
        }//end for 

        // // odd panels
        // #pragma omp parallel for num_threads(8)
        for(auto id : odd_ids){
            run_astar_on_panel(id);
        }*/
    
        // if(level == 8)
        //     break;
    }

    ophidian::routing::AStarRouting astar_routing{m_design};
    for(auto net : astar_nets){
        std::vector<ophidian::routing::AStarSegment> initial_segments;
        for(auto segment : m_design.global_routing().segments(net)) {
            initial_segments.push_back(ophidian::routing::AStarSegment(m_design.global_routing().box(segment), m_design.global_routing().layer_start(segment), m_design.global_routing().layer_end(segment), net));
        }
        m_design.global_routing().unroute(net);
        std::vector<ophidian::routing::AStarSegment> segments;
        auto result = astar_routing.route_net(net, segments);
        if (!result) {
            bool undo = astar_routing.apply_segments_to_global_routing(initial_segments);//This should never fail
            if(undo == false) {
                std::cout<<"WARNING: UNDO ROUTING FAILED, THIS SHOULD NEVER HAPPEN!"<<std::endl;
            }
        }
    }

}//end run_astar_on_panels_parallel

void MCFMultiThreading::run_astar_on_panel(unsigned int panel_id){
 std::vector<ophidian::circuit::Net> nets(m_design.netlist().begin_net(), m_design.netlist().end_net());
     
    auto nets_panel = m_panel_index_to_nets_dict[panel_id];
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

    auto panel_box = m_index_to_panel[panel_id];
    auto panel_min_corner = point_type{ophidian::util::database_unit_t{panel_box.getXl()}, ophidian::util::database_unit_t{panel_box.getYl()}};
    auto panel_max_corner = point_type{ophidian::util::database_unit_t{panel_box.getXh()}, ophidian::util::database_unit_t{panel_box.getYh()}};
    auto panel_region = ophidian::placement::Placement::box_type{panel_min_corner, panel_max_corner};
    auto local_cells = std::vector<ophidian::circuit::CellInstance>{};
    m_design.placement().cells_within(panel_region, local_cells);
   
    auto local_net_cells = std::vector<ophidian::circuit::CellInstance>{};
    for(auto net : nets_local){
        auto net_pins = m_design.netlist().pins(net);
        for(auto net_pin : net_pins){
            auto net_cell = m_design.netlist().cell(net_pin);
            local_net_cells.push_back(net_cell);
        }
    }
    // ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
    // ilpRouting.route_nets_v2(nets_local, local_net_cells, panel_region, fixed_nets, routed_nets, unrouted_nets, movements);
    ophidian::routing::AStarRouting astar_routing{m_design};
    for(auto net : nets_local){
        std::vector<ophidian::routing::AStarSegment> segments;
        astar_routing.route_net(net, segments);
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

    auto wl_after_ilp = m_design.global_routing().wirelength(nets_set_update);  
    m_panel_wirelength_after_ilp_dict[panel_id] = wl_after_ilp;

    for(auto net_tmp : routed_nets){
        m_routed_nets.push_back(net_tmp);
    }
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
    if (DEBUG_PANEL) std::cout << local_cells.size() << " local cells " << std::endl;

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

};//end namespace 
