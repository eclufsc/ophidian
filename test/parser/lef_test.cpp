#include <algorithm>
#include <catch.hpp>

#include <ophidian/parser/Lef.h>

using namespace ophidian;

bool compare(const parser::Lef::site & a, const parser::Lef::site & b)
{
    return a.name == b.name &&
           a.class_ == b.class_ &&
           a.symmetry == b.symmetry &&
           Approx(a.x) == b.x &&
           Approx(a.y) == b.y;
}

bool compare(const parser::Lef::layer & a, const parser::Lef::layer & b)
{
    return a.name == b.name &&
           a.type == b.type &&
           a.direction == b.direction &&
           Approx(a.pitch) == b.pitch &&
           Approx(a.width) == b.width;
}

TEST_CASE("lef: simple.lef parsing", "[lef][lef]") {
    parser::Lef parser("input_files/simple.lef");

    SECTION( "sites are parsed correctly", "[lef][lef]") {
        REQUIRE( parser.sites().size() == 1 );

        parser::Lef::site core;
        core.name = "core";
        core.class_ = "CORE";
        core.x = 0.19;
        core.y = 1.71;

        REQUIRE( compare(parser.sites().front(), core) );
    }

    SECTION( "layers are parsed correctly", "[lef][lef]") {
        std::vector<parser::Lef::layer> layers{
            {"metal1", "ROUTING", parser::Lef::layer::HORIZONTAL,  0.2, 0.1},
            {"metal2", "ROUTING", parser::Lef::layer::VERTICAL,    0.2, 0.1},
            {"metal3", "ROUTING", parser::Lef::layer::HORIZONTAL,  0.2, 0.1}
        };

        REQUIRE( parser.layers().size() == layers.size() );

        for(auto & simple_layer : layers)
        {
            auto comparePredicate = [simple_layer](const parser::Lef::layer & layer) -> bool {
                return compare(simple_layer, layer);
            };

            REQUIRE( std::find_if(
                        parser.layers().begin(),
                        parser.layers().end(),
                        comparePredicate
                    ) != parser.layers().end());
        }
    }

    SECTION( "macros are parsed correctly", "[lef][lef]") {
        REQUIRE( parser.macros().size() == 212 );
        // TODO: Better test for macros
    }
}
