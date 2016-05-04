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
#include "../parsing/def.h"

TEST_CASE("cadence def read", "[cadence][def]") {

    const std::string filename{"input_files/superblue16.def"};
    ophidian::parsing::def def{filename};

    REQUIRE( def.rows().size() == 1788 );
    REQUIRE( def.components().size() == 981559 );
    REQUIRE( def.rows().at(1776).name == "coreROW_1777" );

    REQUIRE( Approx(def.rows().at(1776).origin.x) == 13680.0 );
    REQUIRE( Approx(def.rows().at(1776).origin.y) == 6080760.0 );
    REQUIRE( def.rows().at(1776).num.x == 29658 );
    REQUIRE( def.rows().at(1776).num.y == 1 );
    REQUIRE( def.rows().at(1776).site == "core" );
    REQUIRE( Approx(def.rows().at(1776).step.x) == 380.0 );
    REQUIRE( Approx(def.rows().at(1776).step.y) == 0.0 );

//    o670506 INV_Y1
//       + PLACED ( 74480.000000 2824920.000000 ) N ;
    const ophidian::parsing::def::component * c{nullptr};
    for(auto & component : def.components())
    {
        if(component.name == "o670506")
        {
            c = &component;
            break;
        }
    }

    REQUIRE( c );
    REQUIRE( !c->fixed );
    REQUIRE( Approx(c->position.x) == 74480.0 );
    REQUIRE( Approx(c->position.y) == 2824920.0 );


    REQUIRE( Approx(def.die().lower.x) == 0.0 );
    REQUIRE( Approx(def.die().lower.y) == 0.0 );
    REQUIRE( Approx(def.die().upper.x) == 11283720.0 );
    REQUIRE( Approx(def.die().upper.y) == 6121800.0 );


}
