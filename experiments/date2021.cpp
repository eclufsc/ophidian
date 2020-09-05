#include <catch.hpp>
// #include < stdio.h >
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/GuideWriter.h>
#include <ophidian/parser/DefWriter.h>
#include <ophidian/routing/Engine.h>
#include "run_ilp.h"

bool check_connectivity(const ophidian::design::Design & design, const std::vector<ophidian::circuit::Net>& nets){
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
        if(false){ //debug code
            for(auto gcell : pin_gcells)
            {
                auto box = design.global_routing().gcell_graph()->box(gcell);
                auto layer = design.global_routing().gcell_graph()->layer_index(gcell);
                auto layer_i = design.routing_library().layer_from_index(layer);
                auto layer_str = design.routing_library().name(layer_i);
                std::cout << box.min_corner().x().value() << " " << box.min_corner().y().value() << " " << box.max_corner().x().value() << " " << box.max_corner().y().value() << " " << layer_str << std::endl;
            }
        }

        auto connected = design.global_routing().is_connected(net, pin_gcells, net_name);
        
        if(!connected)
        {
            log() << "net " << net_name << " disconnected with " << design.netlist().pins(net).size() << " pins" << std::endl;
            is_nets_open = true;
        }
    }
    if (is_nets_open) printlog("Open nets detected!"); else printlog("All nets connected!");
    return is_nets_open;
}

TEST_CASE("run ILP for iccad19 benchmarks", "[DATE21]") {

    //iccad 2019 benchmarks
    std::vector<std::string> circuit_names = {
        // "ispd18_sample3",
        // "ispd19_sample4",

        "ispd18_test5",
        // "ispd18_test8",
        // "ispd18_test10",
        // "ispd19_test7",
        // "ispd19_test8",
        // "ispd19_test9"
    };

    // std::string benchmarks_path = "./input_files/ispd19";
    std::string benchmarks_path = "./input_files/circuits";


    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        // std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
        // std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        std::string def_file =   benchmarks_path + "/" + circuit_name + ".input.def";
        std::string lef_file =   benchmarks_path + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + "/cu_gr_solution/" + circuit_name + ".solution_cugr.guide";
        // std::string guide_file = "./" + circuit_name + "_astar.guide";

        ophidian::parser::Def def;
        ophidian::parser::Lef lef;
        ophidian::parser::Guide guide;
        // #pragma omp parallel
        // {
            def = ophidian::parser::Def{def_file};
            lef = ophidian::parser::Lef{lef_file};
            guide = ophidian::parser::Guide{guide_file};
        // }

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);

        UCal::Engine engine(design);
        std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
        // std::vector<ophidian::circuit::Net> nets;
        // auto net2037 = design.netlist().find_net("n_2037");
        // nets.push_back(net2037);
        
        log() << "Initial wirelength = " << design.global_routing().wirelength(nets) << std::endl;
        log() << "A* for generate the initial solution" << std::endl;
        //clear the possible initial solution
        for (auto net : nets) {
            design.global_routing().unroute(net);
        }
        log() << "Cleaned wirelength = " << design.global_routing().wirelength(nets) << std::endl;

        auto start_time = std::chrono::steady_clock::now();
        engine.run_astar_on_circuit(nets);
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end_time-start_time;
        log() << "A* wirelength = " << design.global_routing().wirelength(nets) << std::endl;
        
        log() << "initial solution generated in " << diff.count() << " seconds" << std::endl;


        // check_connectivity(design, nets);
        if(check_connectivity(design, nets)){
            ophidian::parser::write_guide(design, circuit_name + "_astar.guide");
        }

        //ILP lower panels with movement 

        //A* upper panels with movement

        //A* all circuit with movements

        //write new placement in the .def file
        // ophidian::parser::write_replaced_def(design, def_file+"", circuit_name + "_out.def");
        //write GR result in guide file
        // ophidian::parser::write_guide(design, circuit_name + "_out.guide");    
        std::cout << "Memory usage in peak= " << ophidian::util::mem_use::get_peak() << " MB" << std::endl;    
    }
}