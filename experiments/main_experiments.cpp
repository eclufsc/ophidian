#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>
#include <ophidian/util/log.h>
#include "run_ilp.h"
#include <ophidian/parser/GuideWriter.h>
#include <ophidian/parser/DefWriter.h>

using namespace Catch::clara;



int main(int argc, char* argv[]) {
    Catch::Session session;
    int init_size = 0;
    std::string circuit_name = "";
    std::string def_file = "";
    std::string lef_file = "";
    std::string guide_file = "";

    auto cli = session.cli()
        | Opt(circuit_name, "circuit name")
             ["--circuit"]
             ("Circuit Name")
        | Opt(def_file, "Def file path")
             ["--def"]
             ("Def file path")
        | Opt(lef_file, "Lef file path")
             ["--lef"]
             ("Lef file path")
        | Opt(guide_file, "Guide file path")
             ["--guide"]
             ("Guide file path");

    auto result = cli.parse( Args( argc, argv ) );
    if( !result ) {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        exit(1);
    }

    std::cout << "circuit: " << circuit_name << std::endl;

    if(circuit_name == "")
    {
        session.cli(cli);
        auto ret = session.applyCommandLine( argc, argv );
        if (ret) {
            return ret;
        }
        return session.run();
    }else{
        if(def_file == "" || lef_file == "")
        {
            std::cout << "Cannot proceed, missing lef and/or def file name" << std::endl;
            std::cout << "For help, set --help or -help or -h" << std::endl;
            return -1;
        }
        std::cout << "running circuit " << circuit_name << std::endl;

        // std::string benchmarks_path = "./input_files/circuits";
        // std::string def_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.def";
        // std::string lef_file =   benchmarks_path + "/" + circuit_name + "/" + circuit_name + ".input.lef";
        // std::string guide_file = benchmarks_path + "/cu_gr_solution/" + circuit_name + ".solution_cugr.guide";

        ophidian::parser::Def def;
        ophidian::parser::Lef lef;
        ophidian::parser::Guide guide;

        def = ophidian::parser::Def{def_file};
        lef = ophidian::parser::Lef{lef_file};

        bool has_initial_routing = false;
        if(guide_file == "")
        {
            //guide is optional
            guide = ophidian::parser::Guide();
        }else{
            guide = ophidian::parser::Guide{guide_file};
            has_initial_routing = true;
        }

        auto design = ophidian::design::Design();
        ophidian::design::factory::make_design(design, def, lef, guide);
        
        std::vector<std::pair<ophidian::routing::ILPRouting<IloBoolVar>::cell_type, ophidian::routing::ILPRouting<IloBoolVar>::point_type>> movements;
        run_ilp_for_circuit(design, circuit_name, movements, has_initial_routing);

        //replace the cells
        auto & placement = design.placement();
        for(auto movement : movements)
        {
            placement.place(movement.first, movement.second);
        }

        //write new placement in the .def file
        ophidian::parser::write_replaced_def(design, def_file+"", circuit_name + "_out.def");
        //write GR result in guide file
        ophidian::parser::write_guide(design, circuit_name + "_out.guide"); 

        std::cout << "Memory usage in peak= " << ophidian::util::mem_use::get_peak() << " MB" << std::endl;
    }
    return 0;
}
