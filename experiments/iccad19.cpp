#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>

void run_ilp(ophidian::design::Design & design, std::string circuit_name) {
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets, movements);
    std::cout << "result " << result << std::endl;

    if(result){
        // need to generate a new guide file

        // iccad_output_writer.write_ICCAD_2020_output("", movements);
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

    std::string benchmarks_path = "./input_files/circuits/";

    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        std::string def_file = benchmarks_path + circuit_name + ".input.def";
        std::string lef_file = benchmarks_path + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path +"cu_gr_solution/" + circuit_name + ".solution_cugr.guide";

        auto def = ophidian::parser::Def{def_file};
        auto lef = ophidian::parser::Lef{lef_file};
        auto guide = ophidian::parser::Guide{guide_file};

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);

        auto & library = design.routing_library();

        auto & routing_constraint = design.routing_constraints();

        auto net = *design.netlist().begin_net();
        auto netName = design.netlist().name(net);
        
        auto min_layer = routing_constraint.min_net_layer(net);



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