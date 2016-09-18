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

TEST_CASE("VerilogParser: read from stream", "[parser][VerilogParser]")
{
    std::stringstream input("module simple(in, out); input in; output out; endmodule");
    VerilogParser parser;
    auto file = parser.readStream(input);
}

#include <iostream>
TEST_CASE("Verilog::Module: simple test", "[parser][VerilogParser]")
{
    Verilog simpleVerilog;
    auto design = simpleVerilog.addModule("design");
    Verilog::Module *simple = design->addModule("simple");
    auto topLevelInstance = design->addInstance(simple, "design");

    auto inp1Port = simple->addPort(Verilog::PortDirection::INPUT, "inp1");
    auto inp2Port = simple->addPort(Verilog::PortDirection::INPUT, "inp2");
    auto clkPort = simple->addPort(Verilog::PortDirection::INPUT, "iccad_clk");
    auto outPort = simple->addPort(Verilog::PortDirection::OUTPUT, "out");

    auto NAND2 = simple->addModule("NAND2");
    auto NAND2a = NAND2->addPort(Verilog::PortDirection::INPUT, "a");
    auto NAND2b = NAND2->addPort(Verilog::PortDirection::INPUT, "b");
    auto NAND2o = NAND2->addPort(Verilog::PortDirection::OUTPUT, "o");

    auto NOR2 = simple->addModule("NOR2");
    auto NOR2a = NOR2->addPort(Verilog::PortDirection::INPUT, "a");
    auto NOR2b = NOR2->addPort(Verilog::PortDirection::INPUT, "b");
    auto NOR2o = NOR2->addPort(Verilog::PortDirection::OUTPUT, "o");

    auto INV = simple->addModule("INV");
    auto INVa = INV->addPort(Verilog::PortDirection::INPUT, "a");
    auto INVo = INV->addPort(Verilog::PortDirection::OUTPUT, "o");

    auto DFF = simple->addModule("DFF");
    auto DFFck = DFF->addPort(Verilog::PortDirection::INPUT, "ck");
    auto DFFd = DFF->addPort(Verilog::PortDirection::INPUT, "d");
    auto DFFq = DFF->addPort(Verilog::PortDirection::OUTPUT, "q");

    auto inp1 = simple->addNet("inp1");
    auto inp2 = simple->addNet("inp2");
    auto out = simple->addNet("out");
    auto iccad_clk = simple->addNet("iccad_clk");
    auto n1 = simple->addNet("n1");
    auto n2 = simple->addNet("n2");
    auto n3 = simple->addNet("n3");
    auto n4 = simple->addNet("n4");

    Verilog::Instance *u1 = simple->addInstance(NAND2, "u1");
    u1->mapPort(NAND2a, inp1);
    u1->mapPort(NAND2b, inp2);
    u1->mapPort(NAND2o, n1);

    Verilog::Instance *u2 = simple->addInstance(NAND2, "u2");
    u2->mapPort(NOR2a, n1);
    u2->mapPort(NOR2b, n3);
    u2->mapPort(NOR2o, n2);

    Verilog::Instance *f1 = simple->addInstance(DFF, "f1");
    f1->mapPort(DFFck, iccad_clk);
    f1->mapPort(DFFd, n2);
    f1->mapPort(DFFq, n3);

    Verilog::Instance *u3 = simple->addInstance(INV, "u3");
    u3->mapPort(INVa, n3);
    u3->mapPort(INVo, n4);

    Verilog::Instance *u4 = simple->addInstance(INV, "u4");
    u4->mapPort(INVa, n4);
    u4->mapPort(INVo, out);

    REQUIRE( simpleVerilog.modules().size() == 1 );
    REQUIRE( simple->instances().size() == 5 );
    REQUIRE( simple->nets().size() == 8 );
    REQUIRE( simple->ports().size() == 4 );

    topLevelInstance->mapPort(inp1Port, inp1);
    topLevelInstance->mapPort(inp2Port, inp2);
    topLevelInstance->mapPort(clkPort, iccad_clk);
    topLevelInstance->mapPort(outPort, out);

    std::cout << "module " << topLevelInstance->module()->name() << "(";

    std::size_t i = 0;
    for(auto const & port : topLevelInstance->module()->ports())
    {
        std::cout << port.name();
        if(++i == topLevelInstance->module()->ports().size())
        {
            std::cout << ");" << std::endl;
        }
        else
        {
            std::cout << ", ";
        }
    }

    std::cout << "\n// Begin Ports" << std::endl;
    for(auto const & port : topLevelInstance->portMapping())
    {
        std::string portTypeString;
        switch(port.first->direction())
        {
        case Verilog::PortDirection::INPUT:
            portTypeString = "input";
            break;
        case Verilog::PortDirection::OUTPUT:
            portTypeString = "output";
            break;
        case Verilog::PortDirection::INOUT:
            portTypeString = "inout";
            break;
        }
        std::cout << portTypeString << " " << port.first->name() << ";" << std::endl;
    }

    std::cout << "\n// Begin Wires" << std::endl;
    for(auto const & net : simple->nets())
    {
        std::cout << "wire " << net.name() << ";" << std::endl;
    }

    std::cout << "\n// Begin Cells" << std::endl;
    for(auto const & instance : simple->instances())
    {
        std::cout << instance.module()->name() << " " << instance.name();
        std::size_t i = 0;
        for(auto const & port : instance.portMapping())
        {
            std::cout << " ." << port.first->name() << "( " << port.second->name() << " )";
            if(++i == instance.portMapping().size())
            {
                std::cout << ";" << std::endl;
            }
            else
            {
                std::cout << ",";
            }
        }
    }

    std::cout << "endmodule" << std::endl;


}
