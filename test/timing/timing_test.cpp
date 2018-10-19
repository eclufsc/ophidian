#include <catch.hpp>
#include <ophidian/timing/Timer.h>
#include <ophidian/design/DesignFactory.h>

using namespace ophidian;

TEST_CASE("Timer: test of integration with OpenTimer.", "[timing][opentimer]"){
    auto design = design::Design{};

    auto liberty = parser::Liberty("./input_files/tau2015/simple/simple_Early.lib");

    auto verilog = parser::Verilog("./input_files/tau2015/simple/simple.v");

    design::factory::make_design_tau2017(design, liberty, verilog);
    auto openTimer = timing::OpenTimer(design, "./input_files/tau2015/simple/simple_Early.lib", "./input_files/tau2015/simple/simple_Late.lib", "./input_files/tau2015/simple/simple.sdc", "./input_files/tau2015/simple/simple.spef");
    CHECK(openTimer.wns() == Approx(-204.347f));
}
