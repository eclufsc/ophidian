#include <catch.hpp>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/util/Units.h>
// #include <ophidian/util/UtilException.h>


TEST_CASE("GCell Graph Test", "[routing][gcell]")
{
    using dbu = ophidian::util::database_unit_t;


    std::vector<dbu> x = {dbu(0), dbu(200), dbu(500), dbu(800), dbu(1100), dbu(1150)};
    std::vector<dbu> y = {dbu(0), dbu(100), dbu(300), dbu(500), dbu(700), dbu(850)};
    std::vector<int> capacity = {2,4,6};
    //creating a 6x6x3 graph
    auto graph = ophidian::routing::GCellGraph{x,y,3, capacity};

    // SECTION("Try to get node outside graph limits"){
        // CHECK(graph.node(5,5,5) == lemon::INVALID);
    // }
}