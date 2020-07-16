#include "MCFMultiThreading.h"

namespace UCal{

bool DEBUG_MCF_MLT_NET_BOX_RTREE = false;
bool DEBUG_MCF_MLT_CHECK_ROWS = true;
bool DEBUG_MCF_MLT_SPECIFIC_NET = false;

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
    



    construct_net_boxes_rtree(nets);
    if(!DEBUG_MCF_MLT_SPECIFIC_NET)
        cluster_based_on_nets_box();

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

    
        
    number_of_windows = 1;
    for(int i = 0; i < number_of_windows; i++){
        min_x_window = 0;//i*window_size;
        min_y_window = i*window_size;
        max_x_window = layout_width;
        max_y_window = min_y_window + window_size;
        Box2DWrapper window_local(min_x_window,min_y_window,max_x_window,max_y_window);

        if(DEBUG_MCF_MLT_NET_BOX_RTREE) std::cout << "row_" << i << ": " << min_x_window << ", " << min_y_window << ", " << max_x_window << ", " << max_y_window << std::endl;

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
        // std::cout << "number of nets inside window boost: " << nets_local_boost.size() << std::endl;

        // MCFRouting mcf_routing(m_design,nets_local,"case");
        // mcf_routing.route_nets();

        std::vector<ophidian::circuit::Net> fixed_nets;
        std::vector<ophidian::circuit::Net> routed_nets;
        std::vector<ophidian::circuit::Net> unrouted_nets;

        // std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N2003")};
        // std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("N2116")};

        std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;
        std::cout << "routing nets" << std::endl;
        ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(m_design, "case");
        auto result = ilpRouting.route_nets(nets_local, fixed_nets, routed_nets, unrouted_nets, movements);

        std::cout << "result " << result.first << std::endl;
        ophidian::parser::ICCAD2020Writer iccad_output_writer(m_design, "case");

        if(result.first){
            iccad_output_writer.write_ICCAD_2020_output("mcf_output.txt", movements);
        }
   
        //write_statistics_for_circuit(design, circuit_name);

        std::cout << "connected nets" << std::endl;
            for (auto net : nets_local) {
                ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
                for (auto pin : m_design.netlist().pins(net)) {
                    auto pin_name = m_design.netlist().name(pin);                
                    auto location = m_design.placement().location(pin);
                    auto box = ophidian::routing::GCellGraph::box_type{location, location};
                    auto pin_geometry = m_design.placement().geometry(pin);
                    auto layer_name = pin_geometry.front().second;
                    auto pin_layer = m_design.routing_library().find_layer_instance(layer_name);
                    auto layer_index = m_design.routing_library().layerIndex(pin_layer);

                    // std::cout << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

                    m_design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
                }
                auto net_name = m_design.netlist().name(net);
                auto connected = m_design.global_routing().is_connected(net, pin_gcells, net_name);

                if(!connected)
                    std::cout << "net " << net_name << " is open" << std::endl;
            } 



        
    }//end for 

}//end cluster_based_on_nets_box

};//end namespace 
