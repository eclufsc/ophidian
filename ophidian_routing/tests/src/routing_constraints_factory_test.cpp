#include <catch2/catch.hpp>
#include <vector>

#include <ophidian/design/DesignFactory.h>

TEST_CASE("Make a routing constraints from an iccad2020 file.", "[routing][factory]")
{
    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_iccad2020(design, sample);

    auto & netlist = design.netlist();
    auto & global_routing = design.global_routing();
    auto & library = design.routing_library();
    auto & routing_constr = design.routing_constraints();

    SECTION("ndf_constraints")
    {
        REQUIRE(routing_constr.max_cell_movement() == 2);
    }

    SECTION("ndf_constraints")
    {
        REQUIRE(routing_constr.ndf_constraint(1, 1, 0) == 3);
        REQUIRE(routing_constr.ndf_constraint(1, 0, 2) == -2);
        REQUIRE(routing_constr.ndf_constraint(1, 1, 2) == -2);
    }

    SECTION("Net min layer")
    {
        auto net_N1 = netlist.find_net("N1");
        auto min_net_layer_N1 = routing_constr.min_net_layer(net_N1);
        REQUIRE(library.name(min_net_layer_N1) == "M2");
        auto net_N2 = netlist.find_net("N2");
        auto min_net_layer_N2 = routing_constr.min_net_layer(net_N2);
        REQUIRE(library.name(min_net_layer_N2) == "M1");
    }

    SECTION("Extra demand same grid."){
        REQUIRE(routing_constr.extra_demand_same_grid("MC1", "MC2", "M1") == 3);
        REQUIRE(routing_constr.extra_demand_same_grid("MC2", "MC1", "M1") == 3);
        REQUIRE(routing_constr.extra_demand_same_grid("MC3", "MC1", "M1") == 0);
    }

    SECTION("Extra demand adjacent grid."){
        REQUIRE(routing_constr.extra_demand_adj_grid("MC1", "MC2", "M1") == 1);
        REQUIRE(routing_constr.extra_demand_adj_grid("MC2", "MC1", "M1") == 1);
        REQUIRE(routing_constr.extra_demand_adj_grid("MC3", "MC1", "M1") == 0);
    }
}
