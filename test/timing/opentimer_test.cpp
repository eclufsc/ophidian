/*
 * Copyright 2017 Ophidian
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

#include <catch.hpp>
#include <ophidian/timing/OpenTimer.h>
#include <ophidian/timing/OpenTimerWrapper.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/parser/VerilogParser.h>
#include <ophidian/circuit/Verilog2LibraryMapping.h>
#include <ophidian/circuit/Verilog2Netlist.h>

using namespace ophidian;

class OphidianOpenTimerFixture{
public:
    OphidianOpenTimerFixture(): mLibraryMaping(mNetlist){
        parser::VerilogParser verilogParser;
        parser::Verilog * verilog = verilogParser.readFile("input_files/simple.v");
        circuit::verilog2Netlist(*verilog, mNetlist);
        circuit::verilog2LibraryMapping(*verilog, mNetlist, mStandardCells, mLibraryMaping);
        openTimerWrapper.reset(new timing::OpenTimerWrapper(mNetlist, mLibraryMaping, mStandardCells, "input_files/simple.spef", "input_files/simple_Early.lib", "input_files/simple_Late.lib", "input_files/simple.timing"));
    }

    circuit::Netlist mNetlist;
    standard_cell::StandardCells mStandardCells;
    circuit::LibraryMapping mLibraryMaping;
    std::unique_ptr<timing::OpenTimerWrapper> openTimerWrapper;
};

TEST_CASE("Linking and including Opentimer.", "[timer][opentimer]")
{
    std::unique_ptr<timing::opentimer::Timer> timer;
    REQUIRE_NOTHROW(timer = std::move(std::make_unique<timing::opentimer::Timer>()));
    REQUIRE_NOTHROW(timer.reset());
}

TEST_CASE("Adding a cell to Opentimer.", "[timer][opentimer]")
{
    circuit::Netlist netlist;
    timing::opentimer::Timer open_timer;

    netlist.add(circuit::Cell(), "cell");
    open_timer.insert_gate("cell", "std_cell");

    REQUIRE(open_timer.circuit_ptr()->num_gates() == netlist.size(circuit::Cell()));
}

TEST_CASE("Adding Inputs and Outputs to Opentimer.", "[timer][opentimer]")
{
    circuit::Netlist netlist;
    timing::opentimer::Timer open_timer;

    netlist.add(circuit::Input(), netlist.add(circuit::Pin(), "inp"));
    netlist.add(circuit::Output(), netlist.add(circuit::Pin(), "out"));
    open_timer.insert_primary_input("inp");
    open_timer.insert_primary_output("out");

    REQUIRE(open_timer.circuit_ptr()->num_primary_inputs() == netlist.size(circuit::Input()));
    REQUIRE(open_timer.circuit_ptr()->num_primary_outputs() == netlist.size(circuit::Output()));
}

TEST_CASE("Connecting two pins through an interconnection.", "[timer][opentimer]")
{
    circuit::Netlist netlist;
    timing::opentimer::Timer open_timer;

    auto inp = netlist.add(circuit::Pin(), "inp");
    auto out = netlist.add(circuit::Pin(), "out");
    auto net = netlist.add(circuit::Net(), "net");
    netlist.add(circuit::Input(), inp);
    netlist.add(circuit::Output(), out);
    netlist.connect(net, inp);
    netlist.connect(net, out);

    open_timer.insert_primary_input("inp");
    open_timer.insert_primary_output("out");
    open_timer.disconnect_pin("inp");
    open_timer.disconnect_pin("out");
    open_timer.remove_net("inp");
    open_timer.remove_net("out");

    open_timer.insert_net("net");
    open_timer.connect_pin("inp", "net");
    open_timer.connect_pin("out", "net");

    REQUIRE(open_timer.circuit_ptr()->num_nets() == netlist.size(circuit::Net()));
    REQUIRE(open_timer.circuit_ptr()->net_ptr("net")->num_pins() == netlist.pins(net).size());
}

TEST_CASE("Test of Open Timer Wrapper timing.", "[timer][opentimer]")
{
    OphidianOpenTimerFixture ophidianOT;

    REQUIRE(ophidianOT.openTimerWrapper->tns() == Approx(-1652.14));
    REQUIRE(ophidianOT.openTimerWrapper->wns() == Approx(-91.9574));
}
