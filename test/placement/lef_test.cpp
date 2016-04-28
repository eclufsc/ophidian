/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "../catch.hpp"
#include "../placement/lef.h"

using namespace ophidian::placement;
bool compare(const lef_parser::site & a, const lef_parser::site & b)
{
    return a.name == b.name && a.class_ == b.class_ && a.symmetry == b.symmetry && Approx(a.x)==b.x && Approx(a.y)==b.y;
}

bool compare(const lef_parser::layer & a, const lef_parser::layer & b)
{
    return a.name == b.name && a.type == b.type && a.direction == b.direction && Approx(a.pitch) == b.pitch && Approx(a.width) == b.width;
}

#include <algorithm>

bool pin_comparison(const lef_parser::pin & a, const lef_parser::pin & b)
{
    return a.name == b.name;
}

TEST_CASE("Cadence LEF parser","[lef][cadence]")
{
    ophidian::placement::lef_parser parser("benchmarks/superblue18/superblue18.lef");


    lef_parser::site core;
    core.name = "core";
    core.class_ = "CORE";
    core.x = 0.19;
    core.y = 1.71;

    REQUIRE( parser.sites().size() == 1 );
    REQUIRE( compare(parser.sites().front(), core) );

    std::vector<lef_parser::layer> layers{
        {"metal1", "ROUTING", lef_parser::layer::HORIZONTAL, 0.19, 0.065},
        {"metal2", "ROUTING", lef_parser::layer::VERTICAL, 0.19, 0.07},
        {"metal3", "ROUTING", lef_parser::layer::HORIZONTAL, 0.19, 0.07},
        {"metal4", "ROUTING", lef_parser::layer::VERTICAL, 0.285, 0.14}
    };

    REQUIRE( parser.layers().size() == layers.size() );

    for(auto & golden_layer : layers)
    {
        REQUIRE( std::find_if(parser.layers().begin(), parser.layers().end(), [golden_layer](const lef_parser::layer & layer)->bool{
            return compare(golden_layer, layer);
        }) != parser.layers().end());
    }

    REQUIRE( parser.macros().size() == 278 );


    const lef_parser::macro & m = *std::find_if(parser.macros().begin(), parser.macros().end(), [](const lef_parser::macro & m)->bool{
        return m.name == "NAND3_X2";
    });

    lef_parser::macro golden;
    golden.name = "NAND3_X2";
    golden.class_ = "CORE";
    golden.foreign.name = "NAND3_X2";
    golden.foreign.x = 0.0;
    golden.foreign.y = 0.0;
    golden.size.x = 2.660;
    golden.size.y = 1.71;
    golden.site = "core";

    lef_parser::pin goldeno;
    goldeno.name = "o";
    goldeno.direction = lef_parser::pin::OUTPUT;
    lef_parser::port goldenoport;
    goldenoport.layers.push_back("metal1");
    lef_parser::rect goldenoport_rect1{0.975, 0.805, 1.17, 1.26};
    lef_parser::rect goldenoport_rect2{0.975, 0.805, 2.21, 0.87};
    lef_parser::rect goldenoport_rect3{2.035, 0.090, 2.23, 1.26};
    goldenoport.rects.push_back(goldenoport_rect1);
    goldenoport.rects.push_back(goldenoport_rect2);
    goldenoport.rects.push_back(goldenoport_rect3);
    goldeno.ports.push_back(goldenoport);
    golden.pins.push_back(goldeno);


    lef_parser::pin goldena;
    goldena.direction = lef_parser::pin::INPUT;
    lef_parser::port goldenaport;
    goldenaport.layers.push_back("metal1");
    lef_parser::rect goldenaport_rect1{0.090, 0.630, 0.61, 0.76};
    goldenaport.rects.push_back(goldenaport_rect1);
    goldena.ports.push_back(goldenaport);
    golden.pins.push_back(goldena);


    lef_parser::pin goldenb;
    goldenb.name = "b";
    goldenb.direction = lef_parser::pin::INPUT;
    lef_parser::port goldenbport;
    goldenbport.layers.push_back("metal1");
    lef_parser::rect goldenbport_rect1{0.820, 0.090, 1.34, 0.74};
    goldenbport.rects.push_back(goldenbport_rect1);
    goldenb.ports.push_back(goldenbport);
    golden.pins.push_back(goldenb);

    lef_parser::pin goldenc;
    goldenc.name = "c";
    goldenc.direction = lef_parser::pin::INPUT;
    lef_parser::port goldencport;
    goldencport.layers.push_back("metal1");
    lef_parser::rect goldencport_rect1{1.490, 0.090, 1.815, 0.74};
    goldencport.rects.push_back(goldencport_rect1);
    goldenc.ports.push_back(goldencport);
    golden.pins.push_back(goldenc);

    REQUIRE( golden.pins.front().name == goldeno.name );

}

#include "../placement/lef/lef2library.h"
TEST_CASE("lef 2 library", "[lef][cadence]")
{
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::placement::library library{&std_cells};
    ophidian::placement::lef2library({"benchmarks/superblue18/superblue18.lef"}, library);
    ophidian::geometry::point<double> golden_offset(0.5*(1.040+1.495)*library.dist2microns(), 0.5*(0.150+1.255)*library.dist2microns());
    auto offset = library.pin_offset(library.pin_create(library.cell_create("INV_Y3"), "o"));
    REQUIRE( ophidian::geometry::equals(offset, golden_offset) );
}
