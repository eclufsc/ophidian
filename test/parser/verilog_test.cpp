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

#include "verilog_test.h"
#include <catch.hpp>

#include <ophidian/parser/VerilogParser.h>

#include <sstream>

using namespace ophidian::parser;

TEST_CASE("Verilog: add module", "[parser][VerilogParser]")
{
    Verilog file;
    auto m = file.addModule("simple");
    REQUIRE( file.modules().size() == 1 );
    REQUIRE( std::count_if(file.modules().begin(), file.modules().end(), [m](const Verilog::Module & module)->bool{
        return module.name() == m->name();
    }) == 1 );
    REQUIRE( m->name() == "simple" );
}

TEST_CASE("Verilog::Module: add port", "[parser][VerilogParser]")
{
    Verilog::Module simple("simple");
    auto p = simple.addPort(Verilog::PortDirection::INPUT, "inp");
    REQUIRE( simple.ports().size() == 1 );
    REQUIRE( p->name() == "inp" );
    REQUIRE( p->direction() == Verilog::PortDirection::INPUT );
    REQUIRE( std::count_if(simple.ports().begin(), simple.ports().end(), [p](const Verilog::Port & port)->bool{
        return p->name() == port.name() && p->direction() == port.direction();
    }) == 1 );
}

TEST_CASE("Verilog::Module: add net", "[parser][VerilogParser]")
{
    Verilog::Module simple("simple");
    auto n = simple.addNet("inp");
    REQUIRE( simple.nets().size() == 1 );
    REQUIRE( n->name() == "inp" );
    REQUIRE( std::count_if(simple.nets().begin(), simple.nets().end(), [n](const Verilog::Net & net)->bool{
        return n->name() == net.name();
    }) == 1 );
}

TEST_CASE("Verilog::Module: add module", "[parser][VerilogParser]")
{
    Verilog::Module simple("simple");
    auto submodule = simple.addModule("submodule");
    REQUIRE( simple.modules().size() == 1 );
    REQUIRE( submodule->name() == "submodule" );
}

TEST_CASE("Verilog::Module: add instance", "[parser][VerilogParser]")
{
    Verilog::Module simple("simple");
    auto mod = simple.addModule("sub");
    auto inst = simple.addInstance(mod, "u1");
    REQUIRE( inst->module() == mod );
    REQUIRE( inst->name() == "u1" );
    REQUIRE( simple.instances().size() == 1 );
}

TEST_CASE("Verilog::Module: instance port mapping", "[parser][VerilogParser]")
{
    Verilog::Module simple("simple");
    auto inpNet = simple.addNet("inpNet");
    auto outNet = simple.addNet("outNet");

    auto INV = simple.addModule("INV");
    auto INVa = INV->addPort(Verilog::PortDirection::INPUT, "a");
    auto INVo = INV->addPort(Verilog::PortDirection::OUTPUT, "o");

    auto inst = simple.addInstance(INV, "u1");

    inst->mapPort(INVa, inpNet);
    inst->mapPort(INVo, outNet);

    auto mapping = inst->portMapping();

    REQUIRE( mapping[INVa] == inpNet );
    REQUIRE( mapping[INVo] == outNet );
    REQUIRE( mapping.size() == 2 );

}

TEST_CASE("VerilogParser: invalid input", "[parser][VerilogParser]")
{
    std::stringstream input("module simput in; output out; endmodule");
    VerilogParser parser;
    std::unique_ptr<Verilog> file(parser.readStream(input));
    REQUIRE( !file );
}

TEST_CASE("VerilogParser: read module with two ports", "[parser][VerilogParser]")
{
    std::stringstream input("module simple(in, out); input in; output out; endmodule");
    VerilogParser parser;
    std::unique_ptr<Verilog> verilog(parser.readStream(input));
    const Verilog::Module & simple = verilog->modules().front();
    REQUIRE(simple.name() == "simple");
    REQUIRE(simple.ports().size() == 2);
    REQUIRE( std::count(simple.ports().begin(), simple.ports().end(), Verilog::Port(Verilog::PortDirection::INPUT, "in")) == 1 );
    REQUIRE( std::count(simple.ports().begin(), simple.ports().end(), Verilog::Port(Verilog::PortDirection::OUTPUT, "out")) == 1 );
}

TEST_CASE("VerilogParser: read module with two nets", "[parser][VerilogParser]")
{
    std::stringstream input("module simple();  wire netA; wire netB; endmodule");
    VerilogParser parser;
    std::unique_ptr<Verilog> verilog(parser.readStream(input));
    const Verilog::Module & simple = verilog->modules().front();
    REQUIRE(simple.nets().size() == 2);
    REQUIRE( std::count(simple.nets().begin(), simple.nets().end(), Verilog::Net("netA")) == 1 );
    REQUIRE( std::count(simple.nets().begin(), simple.nets().end(), Verilog::Net("netB")) == 1 );
}

TEST_CASE("VerilogParser: read module with one module instanciation", "[parser][VerilogParser]")
{
    std::stringstream input("module simple(inp, out); input inp; output out; wire inp; wire out; INV u1(.a(inp), .o(out)); endmodule");
    VerilogParser parser;
    std::unique_ptr<Verilog> verilog(parser.readStream(input));
    REQUIRE( verilog->modules().size() == 1 );
    const Verilog::Module & simple = verilog->modules().front();
    REQUIRE( simple.modules().size() == 1 );
    const Verilog::Module & INV = simple.modules().front();
    REQUIRE( INV.name() == "INV" );
    REQUIRE( simple.instances().size() == 1 );
    const Verilog::Instance & u1 = simple.instances().front();
    REQUIRE( u1.module() == &INV );
    REQUIRE( u1.name() == "u1" );
    const Verilog::Net & inpNet = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "inp"; });
    const Verilog::Net & outNet = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "out"; });
    const Verilog::Port & INVaPort = *std::find_if(INV.ports().begin(), INV.ports().end(), [](const Verilog::Port& port) ->bool{return port.name() == "a";});
    const Verilog::Port & INVoPort = *std::find_if(INV.ports().begin(), INV.ports().end(), [](const Verilog::Port& port) ->bool{return port.name() == "o";});
    auto portMapping = u1.portMapping();
    REQUIRE( portMapping.size() == INV.ports().size() );
    REQUIRE( portMapping.at(&INVaPort) == &inpNet );
    REQUIRE( portMapping.at(&INVoPort) == &outNet );
}

TEST_CASE("VerilogParser: read simple.v", "[parser][VerilogParser]")
{
    VerilogParser parser;
    std::unique_ptr<Verilog> verilog(parser.readFile("./input_files/simple/simple.v"));
    REQUIRE( verilog );
    REQUIRE( verilog->modules().size() == 1 );

    const Verilog::Module & simple = verilog->modules().front();

    REQUIRE( simple.modules().size() == 5 );

    // #1
    const Verilog::Module & NAND2_X1 = *std::find_if(simple.modules().begin(), simple.modules().end(), [](const Verilog::Module & module)->bool{ return module.name() == "NAND2_X1";});
    const Verilog::Port & NAND2_X1a = *std::find_if(NAND2_X1.ports().begin(), NAND2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "a";});
    const Verilog::Port & NAND2_X1b = *std::find_if(NAND2_X1.ports().begin(), NAND2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "b";});
    const Verilog::Port & NAND2_X1o = *std::find_if(NAND2_X1.ports().begin(), NAND2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "o";});

    // #2
    const Verilog::Module & NOR2_X1 = *std::find_if(simple.modules().begin(), simple.modules().end(), [](const Verilog::Module & module)->bool{ return module.name() == "NOR2_X1";});
    const Verilog::Port & NOR2_X1a = *std::find_if(NOR2_X1.ports().begin(), NOR2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "a";});
    const Verilog::Port & NOR2_X1b = *std::find_if(NOR2_X1.ports().begin(), NOR2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "b";});
    const Verilog::Port & NOR2_X1o = *std::find_if(NOR2_X1.ports().begin(), NOR2_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "o";});

    // #3
    const Verilog::Module & DFF_X80 = *std::find_if(simple.modules().begin(), simple.modules().end(), [](const Verilog::Module & module)->bool{ return module.name() == "DFF_X80";});
    const Verilog::Port & DFF_X80d = *std::find_if(DFF_X80.ports().begin(), DFF_X80.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "d";});
    const Verilog::Port & DFF_X80ck = *std::find_if(DFF_X80.ports().begin(), DFF_X80.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "ck";});
    const Verilog::Port & DFF_X80q = *std::find_if(DFF_X80.ports().begin(), DFF_X80.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "q";});

    // #4
    const Verilog::Module & INV_X1 = *std::find_if(simple.modules().begin(), simple.modules().end(), [](const Verilog::Module & module)->bool{ return module.name() == "INV_X1";});
    const Verilog::Port & INV_X1a = *std::find_if(INV_X1.ports().begin(), INV_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "a";});
    const Verilog::Port & INV_X1o = *std::find_if(INV_X1.ports().begin(), INV_X1.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "o";});

    // #5
    const Verilog::Module & INV_Z80 = *std::find_if(simple.modules().begin(), simple.modules().end(), [](const Verilog::Module & module)->bool{ return module.name() == "INV_Z80";});
    const Verilog::Port & INV_Z80a = *std::find_if(INV_Z80.ports().begin(), INV_Z80.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "a";});
    const Verilog::Port & INV_Z80o = *std::find_if(INV_Z80.ports().begin(), INV_Z80.ports().end(), [](const Verilog::Port & port)->bool{ return port.name() == "o";});

    REQUIRE( simple.instances().size() == 6 );
    const Verilog::Instance & u1 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "u1"; });
    const Verilog::Instance & u2 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "u2"; });
    const Verilog::Instance & f1 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "f1"; });
    const Verilog::Instance & u3 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "u3"; });
    const Verilog::Instance & u4 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "u4"; });
    const Verilog::Instance & lcb1 = *std::find_if(simple.instances().begin(), simple.instances().end(), [](const Verilog::Instance & inst)->bool{ return inst.name() == "lcb1"; });

    REQUIRE( u1.module() == &NAND2_X1 );
    REQUIRE( u2.module() == &NOR2_X1 );
    REQUIRE( f1.module() == &DFF_X80 );
    REQUIRE( u3.module() == &INV_X1 );
    REQUIRE( u4.module() == &INV_X1 );
    REQUIRE( lcb1.module() == &INV_Z80 );

    REQUIRE( simple.nets().size() == 9 );
    const Verilog::Net & inp1 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "inp1"; });
    const Verilog::Net & inp2 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "inp2"; });
    const Verilog::Net & n1 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "n1"; });
    const Verilog::Net & n2 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "n2"; });
    const Verilog::Net & n3 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "n3"; });
    const Verilog::Net & n4 = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "n4"; });
    const Verilog::Net & out = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "out"; });
    const Verilog::Net & iccad_clk = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "iccad_clk"; });
    const Verilog::Net & lcb1_fo = *std::find_if(simple.nets().begin(), simple.nets().end(), [](const Verilog::Net & net)->bool{ return net.name() == "lcb1_fo"; });

    auto u1PortMapping = u1.portMapping();
    REQUIRE( u1PortMapping.size() == 3 );
    REQUIRE( u1PortMapping.at(&NAND2_X1a) == &inp1 );
    REQUIRE( u1PortMapping.at(&NAND2_X1b) == &inp2 );
    REQUIRE( u1PortMapping.at(&NAND2_X1o) == &n1 );

    auto u2PortMapping = u2.portMapping();
    REQUIRE( u2PortMapping.size() == 3 );
    REQUIRE( u2PortMapping.at(&NOR2_X1a) == &n1 );
    REQUIRE( u2PortMapping.at(&NOR2_X1b) == &n3 );
    REQUIRE( u2PortMapping.at(&NOR2_X1o) == &n2 );

    auto f1PortMapping = f1.portMapping();
    REQUIRE( f1PortMapping.size() == 3 );
    REQUIRE( f1PortMapping.at(&DFF_X80d) == &n2 );
    REQUIRE( f1PortMapping.at(&DFF_X80ck) == &lcb1_fo );
    REQUIRE( f1PortMapping.at(&DFF_X80q) == &n3 );

    auto u3PortMapping = u3.portMapping();
    REQUIRE( u3PortMapping.size() == 2 );
    REQUIRE( u3PortMapping.at(&INV_X1a) == &n3 );
    REQUIRE( u3PortMapping.at(&INV_X1o) == &n4 );

    auto u4PortMapping = u4.portMapping();
    REQUIRE( u4PortMapping.size() == 2 );
    REQUIRE( u4PortMapping.at(&INV_X1a) == &n4 );
    REQUIRE( u4PortMapping.at(&INV_X1o) == &out );

    auto lcb1PortMapping = lcb1.portMapping();
    REQUIRE( lcb1PortMapping.size() == 2 );
    REQUIRE( lcb1PortMapping.at(&INV_Z80a) == &iccad_clk );
    REQUIRE( lcb1PortMapping.at(&INV_Z80o) == &lcb1_fo );

}
