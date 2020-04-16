#include <catch.hpp>

#include <ophidian/design/Design.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/parser/ICCAD2020.h>
#include <ophidian/routing/ILPRouting.h>

TEST_CASE("Test for iccad2020 ilp routing.", "[routing][globalRouting][ilp]")
{
    ophidian::parser::ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_iccad2020(design, sample);
    ophidian::routing::ILPRouting ilpRouting(design, "case1");

    std::vector<ophidian::circuit::Net> nets(design.netlist().begin_net(), design.netlist().end_net());
    
    std::cout << "routing nets" << std::endl;
    auto result = ilpRouting.route_nets(nets);
    std::cout << "result " << result << std::endl;
    REQUIRE(result);
}
