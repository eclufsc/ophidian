#include <catch.hpp>
// #include < stdio.h >
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/parser/GuideWriter.h>
#include <ophidian/parser/DefWriter.h>
#include "run_ilp.h"

TEST_CASE("run ILP for iccad19 benchmarks", "[iccad19]") {

    std::vector<std::string> circuit_names = {
        // "ispd18_sample3",
        "ispd19_sample4",
        //"ispd18_test5",
        // "ispd18_test8",
        // "ispd18_test10",
        // "ispd19_test7",
        // "ispd19_test8",
        // "ispd19_test9"

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

    };

    // std::string benchmarks_path = "./input_files/ispd19";
    std::string benchmarks_path = "./input_files/circuits";


    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

        // std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
        // std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        std::string def_file =   benchmarks_path + "/" + circuit_name + ".input.def";
        std::string lef_file =   benchmarks_path + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + "/cu_gr_solution/" + circuit_name + ".solution_cugr_modified.guide";

        // std::cout << def_file << "\n" << lef_file << "\n" << guide_file << std::endl;

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

        // std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
        // for(auto net : nets)
        // {
        //     auto net_name = design.netlist().name(net);
        //     auto wirelength = design.global_routing().wirelength(net);
        //     printflog("Net %s : %d\n", net_name.c_str(), wirelength);
        // }



        // run_ilp_for_circuit(design, circuit_name);

        //write new placement in the .def file
        ophidian::parser::write_replaced_def(design, def_file+"", circuit_name + "_out.def");
        //write GR result in guide file
        ophidian::parser::write_guide(design, circuit_name + "_out.guide");        
    }
}