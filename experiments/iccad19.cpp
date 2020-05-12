#include <catch.hpp>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/ILPRouting.h>

void run_ilp(ophidian::design::Design & design, std::string circuit_name) {
    ophidian::routing::ILPRouting ilpRouting(design, circuit_name);

    // std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    std::vector<ophidian::circuit::Net> nets = {design.netlist().find_net("n_7875")};
    std::vector<ophidian::circuit::Net> fixed_nets;
    std::vector<ophidian::circuit::Net> routed_nets;

    std::vector<std::pair<ophidian::routing::ILPRouting::cell_type, ophidian::routing::ILPRouting::point_type>> movements; 
    std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets, fixed_nets, routed_nets, movements);
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

        using unit_type                        = ophidian::util::database_unit_t;
        using point_type                       = ophidian::util::LocationDbu;
        using box_type                         = ophidian::geometry::Box<unit_type>;
        using gcell_container_type             = std::vector<ophidian::routing::GCell>;

        auto point = point_type(unit_type{255646}, unit_type{436800});
        auto point2 = point_type(unit_type{255647}, unit_type{436801});
        box_type box {point, point2};
        gcell_container_type gcells;
        design.global_routing().gcell_graph()->intersect(gcells, box, 0);

        auto & netlist = design.netlist();
        auto & placement = design.placement();
        auto circuti_die = design.floorplan().chip_upper_right_corner();
        auto net = netlist.find_net("n_7875");

        auto pins = netlist.pins(net);
        for(auto pin : pins)
        {
            auto name = netlist.name(pin);
            auto pos = placement.location(pin);
            auto cell = netlist.cell(pin);
            auto cell_name = netlist.name(cell);
            auto cell_loc = placement.location(cell);
            std::cout << "cell : " << cell_name << " ( " << cell_loc.x().value() << " , " << cell_loc.y().value() << " )" << std::endl;
            std::cout << "pin : " << name << " ( " << pos.x().value() << " , " << pos.y().value() << " )" << std::endl;
        } 


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