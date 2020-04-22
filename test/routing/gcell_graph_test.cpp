#include <catch.hpp>
#include <ophidian/parser/Lef.h>
#include <ophidian/parser/Def.h>
#include <ophidian/parser/Guide.h>
#include <ophidian/parser/ParserException.h>
#include <ophidian/design/DesignFactory.h>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/routing/GlobalRoutingFactory.h>
#include <ophidian/routing/LibraryFactory.h>
#include <ophidian/circuit/StandardCellsFactory.h>
#include <ophidian/circuit/NetlistFactory.h>
#include <ophidian/placement/LibraryFactory.h>
#include <ophidian/placement/PlacementFactory.h>
#include <ophidian/util/Units.h>
#include <ophidian/geometry/Models.h>
#include <algorithm>

using namespace ophidian::parser;

using dbu = ophidian::util::database_unit_t;
using point_type = ophidian::geometry::Point<dbu>;
using box_type = ophidian::geometry::Box<dbu>;



bool boxCompare(const box_type& a, const box_type& b)
{
    return a.min_corner().x() == b.min_corner().x() && a.max_corner().x() == b.max_corner().x() &&
            a.min_corner().y() == b.min_corner().y() && a.max_corner().y() == b.max_corner().y();
}

TEST_CASE("GCell Graph Test", "[routing][gcell]")
{
    /*
       Y
       |
    850|----|-------|-------|-------|----|   
       |    |       |       |       |    |   
    700|----|-------|-------|-------|----|   
       |    |       |       |       |    |   
       |    |       | gcell |       |    |   
    500|----|-------|-------|-------|----|   
       |    |       |       |       |    |   
       |    |       |       |       |    |   
    300|----|-------|-------|-------|----|   
       |    |       |       |       |    |   
       |    |       |       |       |    |   
    100|----|-------|-------|-------|----|   
       |    |       |       |       |    |     
      0---------------------------------------- X
       0   200     500     800    1100  1150
    */
    std::vector<dbu> x = {dbu(0), dbu(200), dbu(500), dbu(800), dbu(1100), dbu(1150)};
    std::vector<dbu> y = {dbu(0), dbu(100), dbu(300), dbu(500), dbu(700), dbu(850)};
    std::vector<int> capacity = {2,4,6};
 
    //creating a 6x6x3 graph
    auto graph = ophidian::routing::GCellGraph{x,y,3, capacity};
    auto gcell = graph.gcell(2,3,1);

    SECTION("verify GCell box"){
        auto box = graph.box(gcell);
        auto expected_box = box_type{point_type{dbu{500}, dbu{500}}, point_type{dbu{800}, dbu{700}}};
        CHECK(boxCompare(box, expected_box));
    }
    SECTION("verify defaut capacity"){
        CHECK(graph.capacity(gcell) == 4);
    }
    SECTION("verify defaut demand"){
        CHECK(graph.demand(gcell) == 0);
    }
    SECTION("increase defaut net demand"){
        graph.change_net_demand(gcell, 1);
        CHECK(graph.net_demand(gcell) == 1);
        graph.change_net_demand(gcell, 3);
        CHECK(graph.net_demand(gcell) == 4);
    }
    SECTION("increase defaut blockage demand"){
        graph.change_blockage_demand(gcell, 1);
        CHECK(graph.blockage_demand(gcell) == 1);
        graph.change_blockage_demand(gcell, 3);
        CHECK(graph.blockage_demand(gcell) == 4);
    }
    SECTION("increase defaut blockage demand"){
        graph.change_blockage_demand(gcell, 1);
        graph.change_blockage_demand(gcell, 2);
        CHECK(graph.demand(gcell) == 3);
    }
    SECTION("intersect with box"){
        auto box = box_type{point_type{dbu{600}, dbu{575}}, point_type{dbu{700}, dbu{675}}};
        std::vector<ophidian::routing::GCell> gcells;     
        graph.intersect(gcells, box, 1);
        CHECK(gcells.size() == 1);
        CHECK(gcells[0] == gcell);
    }
    SECTION("intersect box which is bigger than gcell"){
        auto box = box_type{point_type{dbu{300}, dbu{400}}, point_type{dbu{900}, dbu{800}}};
        std::vector<ophidian::routing::GCell> gcells;
        graph.intersect(gcells, box, 1);
        CHECK(gcells.size() == 9);
        std::vector<ophidian::routing::GCell> expected_gcells;
        expected_gcells.push_back(graph.gcell(1, 2, 1));
        expected_gcells.push_back(graph.gcell(2, 2, 1));
        expected_gcells.push_back(graph.gcell(3, 2, 1));
        expected_gcells.push_back(graph.gcell(1, 3, 1));
        expected_gcells.push_back(gcell); //graph.gcell(2, 3, 1)
        expected_gcells.push_back(graph.gcell(3, 3, 1));
        expected_gcells.push_back(graph.gcell(1, 4, 1));
        expected_gcells.push_back(graph.gcell(2, 4, 1));
        expected_gcells.push_back(graph.gcell(3, 4, 1));
        CHECK(std::is_permutation(gcells.begin(), gcells.end(), expected_gcells.begin()));
    }
    SECTION("intersect box with gcell edge")
    {
        auto box = box_type{point_type{dbu{600}, dbu{500}}, point_type{dbu{700}, dbu{600}}};
        std::vector<ophidian::routing::GCell> gcells;
        graph.intersect(gcells, box, 1);
        CHECK(gcells.size() == 1);
        std::vector<ophidian::routing::GCell> expected_gcells;
        expected_gcells.push_back(gcell);
        expected_gcells.push_back(graph.gcell(2, 3, 1));
        CHECK(std::is_permutation(gcells.begin(), gcells.end(), expected_gcells.begin()));
    }
}

TEST_CASE("Test GCell Capacity", "[routing][gcell][capacity]")
{
    Def sample_def = ophidian::parser::Def{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.def"};
    Lef sample_lef = ophidian::parser::Lef{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.lef"};
    Guide sample_guide = ophidian::parser::Guide{"input_files/ispd19/ispd19_sample4/ispd19_sample4.input.guide"};

    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_ispd2019(design, sample_def, sample_lef, sample_guide);
    auto gcell_graph = design.global_routing().gcell_graph();

    // "Metal 1 Horizontal"
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,0,0)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,1,0)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(1,0,0)) == 10);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(2,2,0)) == 10);
    // upper corner gcell
    CHECK(gcell_graph->capacity(gcell_graph->gcell(196,195,0)) == 2);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,195,0)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(196,0,0)) == 2);

    // "Metal 2 Vertical"
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,0,1)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(1,0,1)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,1,1)) == 10);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(2,2,1)) == 10);
    // upper corner gcell
    CHECK(gcell_graph->capacity(gcell_graph->gcell(196,195,1)) == 8);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,195,1)) == 8);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(196,0,1)) == 1);

    // "Metal 8 Horizontal"
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,0,7)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(1,0,7)) == 1);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,1,7)) == 5);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(3,2,7)) == 5);

    // "Metal 9 Vertical"
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,0,8)) == 0);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(0,1,8)) == 0);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(1,0,8)) == 5);
    CHECK(gcell_graph->capacity(gcell_graph->gcell(2,1,8)) == 5);    
}


TEST_CASE("GCell Graph from iccad2020", "[routing][gcell]")
{
    /* Case1.txt
     * Z = 3 layers
       Y
    50|-------|-------|-------|-------|-------|
      |       |       |       |       |       |
      |       |   C2  |       |       |       |
    40|-------|-------|-------|-------|-------|
      |       |       |       |       |       |
      |   C1  |       |       |   C5  |       |
    30|-------|-------|-------|-------|-------|
      |       |       |       |       |       |
      |       |       |       |       |  C8   |
    20|-------|-------|-------|-------|-------|
      |       |  C3   |       |   C6  |       |
      |       |  C4   |       |   C7  |       |
    10|-------|-------|-------|-------|-------|
      |       |       |       |       |       |
      |       |       |       |       |       |
     0----------------------------------------- X
      0       10      20      30      40      50
    */
    ICCAD2020 sample{"input_files/iccad2020/cases/case1.txt"};
    auto design = ophidian::design::Design{};
    ophidian::design::factory::make_design_iccad2020(design, sample);
    auto & global_routing = design.global_routing();
    auto & netlist = design.netlist();
    auto & placement = design.placement();
    auto gcell_graph = global_routing.gcell_graph();

    SECTION("Graph dimensions")
    {
        REQUIRE(gcell_graph->width() == 5);
        REQUIRE(gcell_graph->height() == 5);
        REQUIRE(gcell_graph->depth() == 3);
    }

    SECTION("GCell box")
    {
        auto gcell_C1 = gcell_graph->gcell(0,3,0);
        auto box_gcell_C1 = box_type{point_type{dbu{0}, dbu{30}},
                                     point_type{dbu{10}, dbu{40}}};
        REQUIRE(boxCompare(box_gcell_C1, gcell_graph->box(gcell_C1)));
    }

    SECTION("GCell default capacity and demand")
    {
        auto gcell_C1 = gcell_graph->gcell(0,3,0);
        REQUIRE(gcell_graph->capacity(gcell_C1) == 10);
        REQUIRE(gcell_graph->demand(gcell_C1) == 2);
        auto gcell_C1_layer3 = gcell_graph->gcell(0,3,2);
        REQUIRE(gcell_graph->capacity(gcell_C1_layer3) == 8);
    }

    SECTION("GCell gcell edge insersection")
    {
        auto gcell_C1 = gcell_graph->gcell(0,3,0);
        auto box_gcell_C1 = gcell_graph->box(gcell_C1);
        std::vector<ophidian::routing::GCell> gcells;
        gcell_graph->intersect(gcells, box_gcell_C1, 0);
        REQUIRE(gcells.size() == 1);
    }

    SECTION("GCell within insersection")
    {
        auto query_box = box_type{point_type{dbu{11}, dbu{11}},
                                  point_type{dbu{19}, dbu{19}}};
        std::vector<ophidian::routing::GCell> gcells;
        gcell_graph->intersect(gcells, query_box, 0);
        REQUIRE(gcells.size() == 1);
    }

    SECTION("Box bigger than a gcell insersection")
    {
        auto query_box = box_type{point_type{dbu{20}, dbu{20}},
                                  point_type{dbu{30}, dbu{50}}};
        std::vector<ophidian::routing::GCell> gcells;
        gcell_graph->intersect(gcells, query_box, 0);
        REQUIRE(gcells.size() == 3);
    }

    SECTION("Get a gcell by cell location")
    {
        auto cell_C3 = netlist.find_cell_instance("C3");
        auto location = placement.location(cell_C3);
        auto gcell_1_1 = gcell_graph->nearest_gcell(location, 0);
        auto box_gcell_1_1 = box_type{point_type{dbu{10}, dbu{10}},
                                      point_type{dbu{20}, dbu{20}}};
        REQUIRE(boxCompare(box_gcell_1_1, gcell_graph->box(gcell_1_1)));
        auto graph_node = gcell_graph->graph_node(gcell_1_1);
        auto node_position = gcell_graph->position(graph_node);
        REQUIRE(node_position.get<0>() == 1);
        REQUIRE(node_position.get<1>() == 1);
        REQUIRE(node_position.get<2>() == 0);
    }
}
