#include <sstream>

#include <catch2/catch.hpp>

#include <ophidian/parser/Verilog.h>
#include <ophidian/parser/ParserException.h>

using ophidian::parser::Verilog;

TEST_CASE("Verilog: missing file", "[parser][verilog]")
{
    CHECK_THROWS_AS(
        Verilog{"thisFileDoesNotExist.lef"},
        ophidian::parser::exceptions::InexistentFile
    );
}

TEST_CASE("Verilog: simple.v", "[parser][verilog][simple]")
{
    auto simple = Verilog{"input_files/simple/simple.v"};

    SECTION("Verilog: check modules size", "[parser][verilog][simple][module][size]"){
        CHECK(simple.modules().size() == 1);
    }

    SECTION("Verilog: check module ports", "[parser][verilog][simple][module][ports]"){
        auto& module = simple.modules().front();

        CHECK(module.name() == "simple");

        CHECK(module.ports().size() == 4);

        auto& first_port = module.ports().front();
        CHECK(first_port.name() == "inp1");
        CHECK(first_port.direction() == Verilog::module_type::port_type::direction_type::INPUT);

        auto& last_port = module.ports().back();
        CHECK(last_port.name() == "out");
        CHECK(last_port.direction() == Verilog::module_type::port_type::direction_type::OUTPUT);
    }

    SECTION("Verilog: check module nets", "[parser][verilog][simple][module][nets]"){
        auto& nets = simple.modules().front().nets();

        CHECK(nets.size() == 9);

        auto& first_net = nets.front();
        CHECK(first_net.name() == "n1");

        auto& last_net = nets.back();
        CHECK(last_net.name() == "lcb1_fo");
    }

    SECTION("Verilog: check module instances", "[parser][verilog][simple][module][instances]"){
        auto& instances = simple.modules().front().module_instances();

        CHECK(instances.size() == 6);

        auto& first_instance = instances.front();
        CHECK(first_instance.name() == "u1");
        CHECK(first_instance.module() == "NAND2_X1");

        auto& first_map = first_instance.net_map();
        CHECK(first_map.at("a") == "inp1");
        CHECK(first_map.at("b") == "inp2");
        CHECK(first_map.at("o") == "n1");

        auto& last_instance = instances.back();
        CHECK(last_instance.name() == "lcb1");
        CHECK(last_instance.module() == "INV_Z80");

        auto& last_map = last_instance.net_map();
        CHECK(last_map.at("a") == "iccad_clk");
        CHECK(last_map.at("o") == "lcb1_fo");
    }
}
