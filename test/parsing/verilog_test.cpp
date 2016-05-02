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

#include "../parsing/verilog.h"

using namespace ophidian;



TEST_CASE("verilog/read","[verilog]") {

    parsing::verilog v("input_files/simple.v");
    REQUIRE( v.design() == "simple" );
    REQUIRE( v.inputs().size() == 3 );
    REQUIRE( v.outputs().size() == 1 );
    REQUIRE( v.wires().size() == 9 );
    REQUIRE( v.modules().size() == 6 );

    const std::vector<std::string> golden_inputs{"inp1", "inp2", "iccad_clk"};
    const std::vector<std::string> golden_outputs{"out"};
    const std::vector<std::string> golden_wires{"n1", "n2", "n3", "n4", "inp1", "inp2", "iccad_clk", "out", "lcb1_fo"};
    std::vector<parsing::verilog::module> golden_modules;
    parsing::verilog::module u1,u2,f1,u3,u4,lcb1;
    u1.type = "NAND2_X1";
    u1.name = "u1";
    u1.pinnet_pairs = {
        {"a", "inp1"},
        {"b", "inp2"},
        {"o", "n1"}
    };

    u2.type = "NOR2_X1";
    u2.name = "u2";
    u2.pinnet_pairs = {
        {"a", "n1"},
        {"b", "n3"},
        {"o", "n2"}
    };

    f1.type = "DFF_X80";
    f1.name = "f1";
    f1.pinnet_pairs = {
        {"d", "n2"},
        {"ck", "lcb1_fo"},
        {"q", "n3"}
    };

    u3.type = "INV_X1";
    u3.name = "u3";
    u3.pinnet_pairs = {
        {"a", "n3"},
        {"o", "n4"}
    };

    u4.type = "INV_X1";
    u4.name = "u4";
    u4.pinnet_pairs = {
        {"a", "n4"},
        {"o", "out"}
    };

    lcb1.type = "INV_Z80";
    lcb1.name = "lcb1";
    lcb1.pinnet_pairs = {
        {"a", "iccad_clk"},
        {"o", "lcb1_fo"}
    };

    golden_modules.push_back(u1);
    golden_modules.push_back(u2);
    golden_modules.push_back(f1);
    golden_modules.push_back(u3);
    golden_modules.push_back(u4);
    golden_modules.push_back(lcb1);

    REQUIRE( v.inputs() == golden_inputs );
    REQUIRE( v.outputs() == golden_outputs );
    REQUIRE( v.modules() == golden_modules );

}

