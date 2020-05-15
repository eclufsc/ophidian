#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>

void run_ilp(ophidian::design::Design & design, std::string circuit_name) {
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);

     //std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("n_1")};
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    int initial_wirelength = design.global_routing().wirelength_in_gcell(nets);
    std::cout << "Circuit initial wirelength = " << initial_wirelength << std::endl;

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    std::cout << "routing nets" << std::endl;
    auto start = std::chrono::high_resolution_clock::now(); 
    auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "result " << result << std::endl;

    if(result){
        // need to generate a new guide file
        std::cout << "Runtime : " << duration.count() << " microsseconds"<< std::endl;
        std::cout << "Runtime : " << std::to_string( (double)duration.count() / 1000000.0 ) << " seconds"<< std::endl;
        int final_wirelength = design.global_routing().wirelength_in_gcell(nets);
        std::cout << "Total movements = " << movements.size() << std::endl;
        std::cout << "Circuit final wirelength = " << final_wirelength << std::endl;
        auto score = initial_wirelength - final_wirelength;
        std::cout << "Estimated score ( "<< initial_wirelength << " - " << final_wirelength << " ) = " << score << std::endl;
        double reduction = 1.0 - ( (double) final_wirelength / (double) initial_wirelength) ;
        std::cout << "% Reduction = " << std::to_string(reduction * 100) << " %" << std::endl;

        std::cout << "\n\n" << 
            circuit_name << ";" <<
            initial_wirelength << ";" <<
            final_wirelength << ";" <<
            score << ";" <<
            reduction << ";" <<
            duration.count() << ";" <<
            movements.size() << "\n\n" << std::endl;


        // iccad_output_writer.write_ICCAD_2020_output("", movements);

        std::cout << "connected nets" << std::endl;
        for (auto net : nets) {
            ophidian::routing::GlobalRouting::gcell_container_type pin_gcells = {};
            for (auto pin : design.netlist().pins(net)) {
                auto location = design.placement().location(pin);
                auto box = ophidian::routing::GCellGraph::box_type{location, location};
                auto pin_geometry = design.placement().geometry(pin);
                auto layer_name = pin_geometry.front().second;
                auto pin_layer = design.routing_library().find_layer_instance(layer_name);
                auto layer_index = design.routing_library().layerIndex(pin_layer);

                design.global_routing().gcell_graph()->intersect(pin_gcells, box, layer_index);
            }
            auto connected = design.global_routing().is_connected(net, pin_gcells);

            auto net_name = design.netlist().name(net);
            std::cout << "net " << net_name << " connected " << connected << std::endl;
        }
    }
   
}

TEST_CASE("run ILP for iccad19 benchmarks", "[iccad19]") {
    std::vector<std::string> circuit_names = {
        "ispd19_sample4",

        // "ispd18_test5",
        // "ispd18_test8",
        // "ispd18_test10",
        // "ispd19_test7",
        // "ispd19_test8",
        // "ispd19_test9"
    };

    std::string benchmarks_path = "./input_files/ispd19/";

    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        std::string def_file = benchmarks_path + circuit_name + "/" + circuit_name + ".input.def";
        std::string lef_file = benchmarks_path + circuit_name + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + circuit_name + "/" + circuit_name + ".solution_cugr.guide";

        auto def = ophidian::parser::Def{def_file};
        auto lef = ophidian::parser::Lef{lef_file};
        auto guide = ophidian::parser::Guide{guide_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);
        


        run_ilp(design, circuit_name);
    }
}



// "ispd18_test1",
// "ispd18_test2",
// "ispd18_test3",
// "ispd18_test4",
// "ispd18_test5",
// "ispd18_test6",
// "ispd18_test7",
// "ispd18_test8",
// "ispd18_test9",
// "ispd18_test10",

// "ispd19_test1",
// "ispd19_test2",
// "ispd19_test3",
// "ispd19_test4",
// "ispd19_test5",
// "ispd19_test6",
// "ispd19_test7",
// "ispd19_test8",
// "ispd19_test9",
// "ispd19_test10",
