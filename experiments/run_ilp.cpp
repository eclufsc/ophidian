
#include "run_ilp.h"

#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/log.h>

using namespace ophidian::util;
bool DEBUG_TEST = true;
void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name, bool initial_routing) {
    if(DEBUG_TEST) log() << "starting function run_ilp_for_circuit" << std::endl;
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(design, circuit_name);
    if(DEBUG_TEST) log() << "create writer" << std::endl;
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    std::vector<std::pair<ophidian::circuit::Net, double>> nets_costs;
    for (auto net : nets) {
        auto pins = design.netlist().pins(net);
        std::vector<ophidian::interconnection::Flute::Point> net_points;
        net_points.reserve(pins.size());
        for (auto pin : pins) {
            auto pin_location = design.placement().location(pin);
            net_points.push_back(pin_location);
        }

        auto & flute = ophidian::interconnection::Flute::instance();
        auto tree = flute.create(net_points);
        auto stwl = tree->length().value();
        stwl /= 10;
        if (stwl == 0) {
            stwl = 1;
        }

        auto routed_length = design.global_routing().wirelength(net);

        auto cost = routed_length / stwl;

        nets_costs.push_back(std::make_pair(net, cost));
    }

    std::sort(nets_costs.begin(), nets_costs.end(), [](std::pair<ophidian::circuit::Net, double> cost_a, std::pair<ophidian::circuit::Net, double> cost_b) {return cost_a.second < cost_b.second;});

    std::vector<ophidian::circuit::Net> nets_to_route;
    std::vector<ophidian::circuit::Net> fixed_nets;
    unsigned number_of_nets = 20000; // overflow in 15320
    unsigned count = 0;
    for (auto net_cost : nets_costs) {        
        if (count < number_of_nets) {
            nets_to_route.push_back(net_cost.first);
        } else {
            fixed_nets.push_back(net_cost.first);
        }
        /*auto net_name = design.netlist().name(net_cost.first);
        if (net_name == "N26") {
            nets_to_route.push_back(net_cost.first);
        } else {
            fixed_nets.push_back(net_cost.first);
        }*/
        count++;
    }

    std::cout << "nets to route " << nets_to_route.size() << std::endl;

    std::vector<ophidian::circuit::Net> routed_nets;
    std::vector<ophidian::circuit::Net> unrouted_nets;

    int initial_wirelength = design.global_routing().wirelength(nets);
    if(DEBUG_TEST) log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    auto ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << " in input file" << std::endl;

    auto demand_before = design.global_routing().gcell_graph()->total_net_demand();
    std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements; 
    if(DEBUG_TEST) log() << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); 
    auto result = ilpRouting.route_nets(nets_to_route, fixed_nets, routed_nets, unrouted_nets, movements, initial_routing);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if(DEBUG_TEST) log() << "result " << result.first << std::endl;

    if(result.first){
        // need to generate a new guide file
        if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength(nets);
        if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
        if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;

        log() <<
            circuit_name << ";" <<
            initial_wirelength << ";" <<
            final_wirelength << ";" <<
            score << ";" <<
            reduction << ";" <<
            movements.size() << ";" << 
            duration.count() << ";" << // total time
            result.second.model_runtime_ms << ";" << //ILP time (ms)
            result.second.model_memory << std::endl; //ILP memory usage

        iccad_output_writer.write_ICCAD_2020_output("RUN_TESTS_OUTPUT.txt", movements);
    }

    // log() << "movements" << std::endl;
    // for (auto movement : movements) {
    //     auto cell = movement.first;
    //     auto cell_name = design.netlist().name(cell);
    //     auto location = movement.second;
    //     log() << "cell " << cell_name << " " << location.x().value() << "," << location.y().value() << std::endl;
    // }

    ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << std::endl;
    ophidian::routing::GlobalRouting::net_container_type ovfl_nets{};
    ovfl = design.global_routing().is_overflow(nets, ovfl_nets) ? "there is overflow" : "No overflow";
    log() << ovfl << std::endl;
    auto demand_after = design.global_routing().gcell_graph()->total_net_demand();
    ophidian::util::mem_use::get_current();
    log() << "Total Demand before: " << demand_before << " Total Demand after: "
    << demand_after << " change: " << demand_before-demand_after << std::endl;
    

    log() << "Checking for disconnected nets..." << std::endl;
    bool is_nets_open = false;
    for (auto net : nets) {
        auto net_name = design.netlist().name(net);
        ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
        for (auto pin : design.netlist().pins(net)) {
            auto pin_name = design.netlist().name(pin);                
            auto location = design.placement().location(pin);
            auto box = ophidian::routing::GCellGraph::box_type{location, location};
            auto pin_geometry = design.placement().geometry(pin);
            auto layer_name = pin_geometry.front().second;
            auto pin_layer = design.routing_library().find_layer_instance(layer_name);
            auto layer_index = design.routing_library().layerIndex(pin_layer);

            // log() << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

            design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
        }
        auto connected = design.global_routing().is_connected(net, pin_gcells, net_name);

        if(!connected)
        {
            log() << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
            is_nets_open = true;
        }
    }
    if (is_nets_open) printlog("Open nets detected!"); else printlog("All nets connected!");
    log() << "MEM: cur=" << mem_use::get_current() << "MB, peak=" << mem_use::get_peak() << "MB"
              << std::endl;
        log() << std::endl;
    printlog("Terminating normally...");
}


void run_circuit(ophidian::design::Design & design, std::string circuit_name) {
    using cell_type = ophidian::circuit::CellInstance;
    using point_type = ophidian::util::LocationDbu;
    using net_type = ophidian::circuit::Net;

    if(DEBUG_TEST) log() << "starting function run_circuit" << std::endl;
    ophidian::routing::ILPRouting<IloBoolVar> ilpRouting(design, circuit_name);
    ophidian::routing::ILPRouting<IloNumVar> lpRouting(design, circuit_name);

    if(DEBUG_TEST) log() << "create writer" << std::endl;
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<net_type> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<net_type> fixed_nets;
    std::vector<net_type> routed_nets;
    std::vector<net_type> unrouted_nets;

    int initial_wirelength = design.global_routing().wirelength(nets);
    if(DEBUG_TEST) log() << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    auto ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    log() << ovfl << " in input file" << std::endl;

    auto demand_before = design.global_routing().gcell_graph()->total_net_demand();
    std::vector<std::pair<cell_type, point_type>> movements; 
    if(DEBUG_TEST) log() << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); 
    auto result = lpRouting.route_nets(nets, fixed_nets, routed_nets, unrouted_nets, movements);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if(DEBUG_TEST) log() << "result " << result.first << std::endl;


    if(result.first){
        // need to generate a new guide file
        if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength(nets);
        if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
        if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
    }

    auto nets_size = nets.size();
    auto routed_size = routed_nets.size();
    auto unrouted_size = unrouted_nets.size();

    if(nets_size != (routed_size + unrouted_size))
        log() << "NETS SIZE WRONG!!" << std::endl;
        

    if(unrouted_size > 0)
    {
        log();
        log() << "Initing second iteration ..." << std::endl;
        log();

        start = std::chrono::high_resolution_clock::now(); 
        auto result2 = ilpRouting.route_nets(nets, fixed_nets, routed_nets, unrouted_nets, movements);
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        log() << "result " << result2.first << std::endl;

        if(result.first){
            // need to generate a new guide file
            if(DEBUG_TEST) log() << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
            if(DEBUG_TEST) log() << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
            int final_wirelength = design.global_routing().wirelength(nets);
            if(DEBUG_TEST) log() << "Total movements = " << movements.size() << std::endl;
            if(DEBUG_TEST) log() << "Circuit final wirelength = " << final_wirelength << std::endl;
            auto score = initial_wirelength - final_wirelength;
            if(DEBUG_TEST) log() << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
            double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
            if(DEBUG_TEST) log() << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;
        }
    }

}