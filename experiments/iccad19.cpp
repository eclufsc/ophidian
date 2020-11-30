#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>

TEST_CASE("run ILP for iccad19 benchmarks", "[iccad19]") {

    //iccad 2019 benchmarks
    std::vector<std::string> circuit_names = {
         "ispd18_test1",
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
        //"ispd19_test8",
        //"ispd19_test9",
        //"ispd19_test10",
    };

    std::string benchmarks_path = "./input_files/circuits";

    for (auto circuit_name : circuit_names) {
        std::cout << "running circuit " << circuit_name << std::endl;

         std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
         std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        //std::string def_file =   benchmarks_path + "/" + circuit_name + ".input.def";
        //std::string lef_file =   benchmarks_path + "/" + circuit_name + ".input.lef";
        std::string guide_file = benchmarks_path + "/cu_gr_solution/" + circuit_name + ".solution_cugr.nopatch.guide";
        // std::string guide_file = benchmarks_path + "/cugr_nopatch/" + circuit_name + "/" + circuit_name + ".cugr.nopatch.output.guide";
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
    }
}


