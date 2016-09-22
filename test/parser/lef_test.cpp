#include <catch.hpp>

#include <ophidian/parser/Lef.h>
#include <iostream>

using namespace ophidian;

bool operator==(const parser::Lef::site & a, const parser::Lef::site & b)
{
    return a.name == b.name &&
           a.class_ == b.class_ &&
           a.symmetry == b.symmetry &&
           Approx(a.x) == b.x &&
           Approx(a.y) == b.y;
}

bool operator==(const parser::Lef::layer & a, const parser::Lef::layer & b)
{
    return a.name == b.name &&
           a.type == b.type &&
           a.direction == b.direction &&
           Approx(a.pitch) == b.pitch &&
           Approx(a.width) == b.width;
}

bool operator==(const parser::Lef::rect & a, const parser::Lef::rect & b)
{
    return Approx(a.xl) == b.xl &&
           Approx(a.yl) == b.yl &&
           Approx(a.xh) == b.xh &&
           Approx(a.yh) == b.yh;
}

bool operator==(const std::vector<parser::Lef::rect> & a, const std::vector<parser::Lef::rect> & b)
{
    bool rectsAreEqual = true;

    for (auto& i : a) {
        auto comparePredicate = [i](const parser::Lef::rect & layer) -> bool {
            return i == layer;
        };

        rectsAreEqual = rectsAreEqual && std::find_if(a.begin(), a.end(), comparePredicate) != a.end();
    }

    return rectsAreEqual;
}

bool operator==(const std::vector<std::string> & a, const std::vector<std::string> & b)
{
    bool layersAreEqual = true;

    for (auto& i : a) {
        auto comparePredicate = [i](const std::string & layer) -> bool {
            return i == layer;
        };

        layersAreEqual = layersAreEqual && std::find_if(a.begin(), a.end(), comparePredicate) != a.end();
    }

    return layersAreEqual;
}

bool operator==(const parser::Lef::port & a, const parser::Lef::port & b)
{
    return a.layers == b.layers &&
           a.rects == b.rects;
}

bool operator==(const std::vector<parser::Lef::port> & a, const std::vector<parser::Lef::port> & b)
{
    bool portsAreEqual = true;

    for (auto& i : a) {
        auto comparePredicate = [i](const parser::Lef::port & port) -> bool {
            return i == port;
        };

        portsAreEqual = portsAreEqual && std::find_if(a.begin(), a.end(), comparePredicate) != a.end();
    }

    return portsAreEqual;
}

bool operator==(const parser::Lef::pin & a, const parser::Lef::pin & b)
{
    return a.name == b.name &&
           a.direction == b.direction &&
           a.ports == b.ports;
}

bool operator==(const parser::Lef::macro_size & a, const parser::Lef::macro_size & b)
{
    return Approx(a.x) == b.x &&
           Approx(a.y) == b.y;
}

bool operator==(const parser::Lef::macro_foreign & a, const parser::Lef::macro_foreign & b)
{
    return a.name == b.name &&
           Approx(a.x) == b.x &&
           Approx(a.y) == b.y;
}

bool operator==(const std::vector<parser::Lef::pin> & a, const std::vector<parser::Lef::pin> & b)
{
    bool pinsAreEqual = true;

    for (auto& i : a) {
        auto comparePredicate = [i](const parser::Lef::pin & pin) -> bool {
            return i == pin;
        };

        pinsAreEqual = pinsAreEqual && std::find_if(a.begin(), a.end(), comparePredicate) != a.end();
    }

    return pinsAreEqual;
}

bool operator==(const parser::Lef::macro & a, const parser::Lef::macro & b)
{
    return a.name == b.name &&
           a.class_ == b.class_ &&
           a.pins == b.pins &&
           a.foreign == b.foreign &&
           a.size == b.size &&
           a.site == b.site &&
           // a.obses, b.obses) &&
           a.origin == b.origin;
}

TEST_CASE("lef: simple.lef parsing", "[lef][lef]") {
    parser::Lef parser("input_files/simple.lef");

    SECTION( "sites are parsed correctly", "[lef][lef]") {
        CHECK( parser.sites().size() == 1 );

        parser::Lef::site core;
        core.name = "core";
        core.class_ = "CORE";
        core.x = 0.19;
        core.y = 1.71;

        REQUIRE(parser.sites().front() == core);
    }

    SECTION( "layers are parsed correctly", "[lef][lef]") {
        std::vector<parser::Lef::layer> layers {
            {"metal1", "ROUTING", parser::Lef::layer::HORIZONTAL,  0.2, 0.1},
            {"metal2", "ROUTING", parser::Lef::layer::VERTICAL,    0.2, 0.1},
            {"metal3", "ROUTING", parser::Lef::layer::HORIZONTAL,  0.2, 0.1}
        };

        CHECK( parser.layers().size() == layers.size() );

        for(auto & simple_layer : layers)
        {
            auto comparePredicate = [simple_layer](const parser::Lef::layer & layer) -> bool {
                            return simple_layer == layer;
                        };

            REQUIRE( std::find_if(
                     parser.layers().begin(),
                     parser.layers().end(),
                     comparePredicate
                     ) != parser.layers().end());
        }
    }

    SECTION( "macros are parsed correctly", "[lef][lef]") {
        CHECK( parser.macros().size() == 212 );

        std::vector<std::string> m1_pin_layers = {"metal1"};

        std::vector<parser::Lef::rect> m1_o_rects = {
            {0.465, 0.150, 0.53, 1.255},
            {0.415, 0.150, 0.61, 0.28}
        };

        std::vector<parser::Lef::rect> m1_a_rects = {
                {0.210, 0.340, 0.34, 0.405}
        };

        parser::Lef::port m1_o_port = {m1_pin_layers, m1_o_rects};
        parser::Lef::port m1_a_port = {m1_pin_layers, m1_a_rects};

        parser::Lef::pin o = {"o", parser::Lef::pin::OUTPUT, {m1_o_port}};
        parser::Lef::pin a = {"a", parser::Lef::pin::INPUT, {m1_a_port}};

        parser::Lef::macro_foreign m1_foreign = {"INV_X1", 0.000, 0.000};

        parser::Lef::macro m1;
        m1.name = "INV_X1";
        m1.class_ = "CORE";
        m1.pins = {o, a};
        m1.foreign = m1_foreign;
        m1.size = {0.760, 1.71};
        m1.site = "core";
        // m1.obses = ?;
        m1.origin = {0.000, 0.000};

        REQUIRE( parser.macros().front() == m1);

        // TODO: Check macro obses
    }
}