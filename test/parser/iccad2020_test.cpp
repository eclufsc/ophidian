#include <catch.hpp>
#include <algorithm>

#include <ophidian/parser/ICCAD2020.h>

using namespace ophidian::parser;

TEST_CASE("ICCAD2020: read case1.txt file", "[parser][iccad2020]")
{
    ICCAD2020 sample = ICCAD2020{"input_files/iccad2020/case1.txt"};

    SECTION("Max Constraint movement"){
        REQUIRE(sample.max_cell_move() == 2);
    }

    SECTION("Graph dimensions"){
        auto grid_origin = sample.grid_origin();
        auto grid_boundary = sample.grid_boundary();
        REQUIRE(grid_origin == std::make_pair(1, 1));
        REQUIRE(grid_boundary == std::make_pair(5, 5));
        REQUIRE(sample.grid_dimensions() == std::make_tuple<int, int, int>(5, 5, 3));
    }

    SECTION("Layer features"){
        auto layers = sample.layers();
        REQUIRE(layers.size() == 3);

        auto layer_M1_it = std::find_if(layers.begin(), layers.end(), [](auto layer){return layer.name() == "M1";});
        auto layer_M2_it = std::find_if(layers.begin(), layers.end(), [](auto layer){return layer.name() == "M2";});
        auto layer_M3_it = std::find_if(layers.begin(), layers.end(), [](auto layer){return layer.name() == "M3";});

        REQUIRE(layer_M1_it->name() == "M1");
        REQUIRE(layer_M2_it->name() == "M2");
        REQUIRE(layer_M3_it->name() == "M3");

        REQUIRE(layer_M1_it->direction() == ophidian::parser::Layer::Direction::HORIZONTAL);
        REQUIRE(layer_M2_it->direction() == ophidian::parser::Layer::Direction::VERTICAL);
        REQUIRE(layer_M3_it->direction() == ophidian::parser::Layer::Direction::HORIZONTAL);

        REQUIRE(layer_M1_it->index() == 1);
        REQUIRE(layer_M2_it->index() == 2);
        REQUIRE(layer_M3_it->index() == 3);

        REQUIRE(layer_M1_it->capacity() == 10);
        REQUIRE(layer_M2_it->capacity() == 8);
        REQUIRE(layer_M3_it->capacity() == 8);
    }

    SECTION("Non default routing constraints"){
        auto ndf_constraints = sample.gcell_non_default_supply();
        REQUIRE(ndf_constraints.size() == 3);
        std::vector<std::pair<std::tuple<int, int, int>, int>> expected_constraints = {{{2,2,1}, 3}, {{2,1,3}, -2}, {{2,2,3}, -2}};
        REQUIRE(std::is_permutation(ndf_constraints.begin(), ndf_constraints.end(), expected_constraints.begin()));
    }

    SECTION("Macros and their pins and blockages"){
        auto & macros = sample.macros();
        REQUIRE(macros.size() == 3);

        auto macro_mc1_it = std::find_if(macros.begin(), macros.end(), [](auto & macro){return macro.name() == "MC1";});
        REQUIRE(macro_mc1_it->name() == "MC1");

        REQUIRE(macro_mc1_it->pins().size() == 2);
        std::vector<std::string> mc1_pin_names, expected_mc1_pin_names={"P1", "P2"};
        for(auto & mc1_pin : macro_mc1_it->pins())
            mc1_pin_names.push_back(mc1_pin.name());
        REQUIRE(std::is_permutation(mc1_pin_names.begin(), mc1_pin_names.end(), expected_mc1_pin_names.begin()));

        std::vector<std::string> mc1_pin_layers, expected_mc1_pin_layers={"M1", "M1"};
        for(auto & mc1_pin : macro_mc1_it->pins())
            mc1_pin_layers.push_back(mc1_pin.layer());
        REQUIRE(std::is_permutation(mc1_pin_layers.begin(), mc1_pin_layers.end(), expected_mc1_pin_layers.begin()));

        auto mc1_blockages = sample.blockages("MC1");
        REQUIRE(mc1_blockages.size() == 2);
        auto mc2_blockages = sample.blockages("MC2");
        REQUIRE(mc2_blockages.size() == 0);

        std::vector<std::string> mc1_blk_names, expected_mc1_blk_names={"B1", "B2"};
        for(auto blk : mc1_blockages)
            mc1_blk_names.push_back(blk.blockage_name);
        REQUIRE(std::is_permutation(mc1_blk_names.begin(), mc1_blk_names.end(), expected_mc1_blk_names.begin()));

        auto blk_B1_it = std::find_if(mc1_blockages.begin(), mc1_blockages.end(), [&](auto & blk){return blk.blockage_name == "B1";});
        REQUIRE(blk_B1_it->macro_name == "MC1");
        REQUIRE(blk_B1_it->layer_name == "M1");
        REQUIRE(blk_B1_it->demand == 2);
    }

    SECTION("Same grid extra demand constraint"){
        auto same_grid_extra_demands = sample.same_grid_extra_demands();
        REQUIRE(same_grid_extra_demands.size() == 1);
        auto same_grid_extra_demand = same_grid_extra_demands.at(0);
        REQUIRE(same_grid_extra_demand.macro1_name == "MC1");
        REQUIRE(same_grid_extra_demand.macro2_name == "MC2");
        REQUIRE(same_grid_extra_demand.layer_name == "M1");
        REQUIRE(same_grid_extra_demand.extra_demand == 3);
    }

    SECTION("Adjacent grid extra demand constraint"){
        auto adj_grid_extra_demands = sample.adj_grid_extra_demands();
        REQUIRE(adj_grid_extra_demands.size() == 1);
        auto adj_grid_extra_demand = adj_grid_extra_demands.at(0);
        REQUIRE(adj_grid_extra_demand.macro1_name == "MC1");
        REQUIRE(adj_grid_extra_demand.macro2_name == "MC2");
        REQUIRE(adj_grid_extra_demand.layer_name == "M1");
        REQUIRE(adj_grid_extra_demand.extra_demand == 1);
    }

    SECTION("Components"){
        auto components = sample.components();
        REQUIRE(components.size() == 8);
        std::vector<std::string> component_names, expected_component_names={"C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8"};
        for(auto component : components)
            component_names.push_back(component.name());
        REQUIRE(std::is_permutation(component_names.begin(), component_names.end(), expected_component_names.begin()));

        auto component_C6_it = std::find_if(components.begin(), components.end(), [](auto & component){return component.name() == "C6";});
        REQUIRE(component_C6_it->name() == "C6");
        REQUIRE(component_C6_it->macro() == "MC2");
        REQUIRE(component_C6_it->fixed() == false);
        auto position = ophidian::parser::Component::database_unit_point_type{ophidian::parser::Component::database_unit_type{2}, ophidian::parser::Component::database_unit_type{2}};
        ophidian::parser::Component::database_unit_type x{4}, y{2};
        REQUIRE(component_C6_it->position().x() == x);
        REQUIRE(component_C6_it->position().y() == y);
    }

    SECTION("Nets"){
        auto nets = sample.nets();
        REQUIRE(nets.size() == 6);
        std::vector<std::string> net_names, expected_net_names={"N1", "N2", "N3", "N4", "N5", "N6"};
        for(auto net : nets)
            net_names.push_back(net.name());
        REQUIRE(std::is_permutation(net_names.begin(), net_names.end(), expected_net_names.begin()));

        auto net_N1_it = std::find_if(nets.begin(), nets.end(), [](auto & net){return net.name() == "N1";});
        auto net_N2_it = std::find_if(nets.begin(), nets.end(), [](auto & net){return net.name() == "N2";});

        REQUIRE(net_N1_it->min_layer() == "M2");
        REQUIRE(net_N2_it->min_layer() == "NoCstr");

        auto n1_pins = net_N1_it->pins();
        std::vector<std::pair<std::string, std::string>> net_elements, expected_net_elements{{"C1", "P2"}, {"C5", "P2"}, {"C4", "P2"}};
        for(auto pin_pair : n1_pins)
            net_elements.push_back(pin_pair);
        REQUIRE(std::is_permutation(net_elements.begin(), net_elements.end(), expected_net_elements.begin()));
    }

    SECTION("Routing segments"){
        auto segments_N1 = sample.segments("N1");
        REQUIRE(segments_N1.size() == 10);

        auto segments_invalid = sample.segments("invalid");
        REQUIRE(segments_invalid.size() == 0);
    }
}
