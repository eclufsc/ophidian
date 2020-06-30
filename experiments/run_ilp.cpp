
#include "run_ilp.h"

#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/ICCAD2020Writer.h>
#include <ophidian/util/Log.h>

bool DEBUG_TEST = true;
void run_ilp_for_circuit(ophidian::design::Design & design, std::string circuit_name, bool initial_routing) {
    if(DEBUG_TEST) std::cout << "starting function nun ILP" << std::endl;
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);
    if(DEBUG_TEST) std::cout << "create writer" << std::endl;
    ophidian::parser::ICCAD2020Writer iccad_output_writer(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    int initial_wirelength = design.global_routing().wirelength_in_gcell(nets);
    if(DEBUG_TEST) std::cout << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    auto ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    std::cout << ovfl << " in input file" << std::endl;

    auto demand_before = design.global_routing().gcell_graph()->total_demand();
    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    if(DEBUG_TEST) std::cout << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); 
    auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements, true, initial_routing);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    if(DEBUG_TEST) std::cout << "result " << result.first << std::endl;

    if(result.first){
        // need to generate a new guide file
        if(DEBUG_TEST) std::cout << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        if(DEBUG_TEST) std::cout << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength_in_gcell(nets);
        if(DEBUG_TEST) std::cout << "Total movements = " << movements.size() << std::endl;
        if(DEBUG_TEST) std::cout << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        if(DEBUG_TEST) std::cout << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        if(DEBUG_TEST) std::cout << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;

        std::cout << "\n\n" << 
            circuit_name << ";" <<
            initial_wirelength << ";" <<
            final_wirelength << ";" <<
            score << ";" <<
            reduction << ";" <<
            movements.size() << ";" << 
            duration.count() << ";" << // total time
            result.second.model_runtime_ms << ";" << //ILP time (ms)
            result.second.model_memory << "\n\n" << std::endl; //ILP memory usage

        iccad_output_writer.write_ICCAD_2020_output("RUN_TESTS_OUTPUT.txt", movements);
    }

    // std::cout << "movements" << std::endl;
    // for (auto movement : movements) {
    //     auto cell = movement.first;
    //     auto cell_name = design.netlist().name(cell);
    //     auto location = movement.second;
    //     std::cout << "cell " << cell_name << " " << location.x().value() << "," << location.y().value() << std::endl;
    // }

    ovfl = design.global_routing().is_overflow() ? "there is overflow" : "No overflow";
    std::cout << ovfl << std::endl;
    ophidian::routing::GlobalRouting::net_container_type ovfl_nets{};
    ovfl = design.global_routing().is_overflow(nets, ovfl_nets) ? "there is overflow" : "No overflow";
    std::cout << ovfl << std::endl;
    auto demand_after = design.global_routing().gcell_graph()->total_demand();
    std::cout << "Total Demand before: " << demand_before << " Total Demand after: "
    << demand_after << " change: " << demand_before-demand_after << std::endl;
    

    std::cout << "disconnected nets:" << std::endl;
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

            // std::cout << "pin " << pin_name << " layer " << layer_name << " index " << layer_index << std::endl;

            design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index-1);
        }
        auto connected = design.global_routing().is_connected(net, pin_gcells, net_name);

        if(!connected)
            std::cout << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
    }

    std::cout << "Memory usage in peak= " << ophidian::util::MemoryUsage::get_peak() << " MB" << std::endl;
}
