/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "../catch.hpp"
#include "../parsing/lef.h"

using namespace ophidian;
bool compare(const parsing::lef::site & a, const parsing::lef::site & b)
{
    return a.name == b.name && a.class_ == b.class_ && a.symmetry == b.symmetry && Approx(a.x)==b.x && Approx(a.y)==b.y;
}

bool compare(const parsing::lef::layer & a, const parsing::lef::layer & b)
{
    return a.name == b.name && a.type == b.type && a.direction == b.direction && Approx(a.pitch) == b.pitch && Approx(a.width) == b.width;
}

#include <algorithm>

bool pin_comparison(const parsing::lef::pin & a, const parsing::lef::pin & b)
{
    return a.name == b.name;
}

TEST_CASE("Cadence LEF parser","[lef][cadence]")
{
    parsing::lef parser("input_files/superblue18.lef");

    parsing::lef::site core;
    core.name = "core";
    core.class_ = "CORE";
    core.x = 0.19;
    core.y = 1.71;

    REQUIRE( parser.sites().size() == 1 );
    REQUIRE( compare(parser.sites().front(), core) );

    std::vector<parsing::lef::layer> layers{
        {"metal1", "ROUTING", parsing::lef::layer::HORIZONTAL, 0.19, 0.065},
        {"metal2", "ROUTING", parsing::lef::layer::VERTICAL, 0.19, 0.07},
        {"metal3", "ROUTING", parsing::lef::layer::HORIZONTAL, 0.19, 0.07},
        {"metal4", "ROUTING", parsing::lef::layer::VERTICAL, 0.285, 0.14}
    };

    REQUIRE( parser.layers().size() == layers.size() );

    for(auto & golden_layer : layers)
    {
        REQUIRE( std::find_if(parser.layers().begin(), parser.layers().end(), [golden_layer](const parsing::lef::layer & layer)->bool{
            return compare(golden_layer, layer);
        }) != parser.layers().end());
    }

    REQUIRE( parser.macros().size() == 278 );


    const parsing::lef::macro & m = *std::find_if(parser.macros().begin(), parser.macros().end(), [](const parsing::lef::macro & m)->bool{
        return m.name == "NAND3_X2";
    });

    parsing::lef::macro golden;
    golden.name = "NAND3_X2";
    golden.class_ = "CORE";
    golden.foreign.name = "NAND3_X2";
    golden.foreign.x = 0.0;
    golden.foreign.y = 0.0;
    golden.size.x = 2.660;
    golden.size.y = 1.71;
    golden.site = "core";

    parsing::lef::pin goldeno;
    goldeno.name = "o";
    goldeno.direction = parsing::lef::pin::OUTPUT;
    parsing::lef::port goldenoport;
    goldenoport.layers.push_back("metal1");
    parsing::lef::rect goldenoport_rect1{0.975, 0.805, 1.17, 1.26};
    parsing::lef::rect goldenoport_rect2{0.975, 0.805, 2.21, 0.87};
    parsing::lef::rect goldenoport_rect3{2.035, 0.090, 2.23, 1.26};
    goldenoport.rects.push_back(goldenoport_rect1);
    goldenoport.rects.push_back(goldenoport_rect2);
    goldenoport.rects.push_back(goldenoport_rect3);
    goldeno.ports.push_back(goldenoport);
    golden.pins.push_back(goldeno);


    parsing::lef::pin goldena;
    goldena.direction = parsing::lef::pin::INPUT;
    parsing::lef::port goldenaport;
    goldenaport.layers.push_back("metal1");
    parsing::lef::rect goldenaport_rect1{0.090, 0.630, 0.61, 0.76};
    goldenaport.rects.push_back(goldenaport_rect1);
    goldena.ports.push_back(goldenaport);
    golden.pins.push_back(goldena);


    parsing::lef::pin goldenb;
    goldenb.name = "b";
    goldenb.direction = parsing::lef::pin::INPUT;
    parsing::lef::port goldenbport;
    goldenbport.layers.push_back("metal1");
    parsing::lef::rect goldenbport_rect1{0.820, 0.090, 1.34, 0.74};
    goldenbport.rects.push_back(goldenbport_rect1);
    goldenb.ports.push_back(goldenbport);
    golden.pins.push_back(goldenb);

    parsing::lef::pin goldenc;
    goldenc.name = "c";
    goldenc.direction = parsing::lef::pin::INPUT;
    parsing::lef::port goldencport;
    goldencport.layers.push_back("metal1");
    parsing::lef::rect goldencport_rect1{1.490, 0.090, 1.815, 0.74};
    goldencport.rects.push_back(goldencport_rect1);
    goldenc.ports.push_back(goldencport);
    golden.pins.push_back(goldenc);

    REQUIRE( golden.pins.front().name == goldeno.name );

}

#include "../placement/lef2library.h"
TEST_CASE("lef 2 library", "[lef][cadence]")
{
    ophidian::standard_cell::standard_cells std_cells;
    ophidian::placement::library library{&std_cells};
    ophidian::placement::lef2library({"input_files/superblue18.lef"}, library);
    ophidian::geometry::point<double> golden_offset(0.5*(1.040+1.495)*library.dist2microns(), 0.5*(0.150+1.255)*library.dist2microns());
    auto offset = library.pin_offset(library.pin_create(library.cell_create("INV_Y3"), "o"));
    REQUIRE( ophidian::geometry::equals(offset, golden_offset) );
}
