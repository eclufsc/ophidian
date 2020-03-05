#include <catch.hpp>
#include <ophidian/routing/GCellGraph.h>
#include <ophidian/util/Units.h>
#include <ophidian/geometry/Models.h>

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
    SECTION("increase defaut demand"){
        graph.increase_demand(gcell);
        CHECK(graph.demand(gcell) == 1);
    }
    SECTION("intersect with box"){
        auto box = box_type{point_type{dbu{600}, dbu{575}}, point_type{dbu{700}, dbu{675}}};
        std::vector<ophidian::routing::GCell> gcells;     
        graph.intersect(gcells, box, 1);
        CHECK(gcells.size() == 1);
        CHECK(gcells[0] == gcell);
    }
    SECTION("intersect box which is bigguer tha gcell"){
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
        CHECK(gcells.size() == 2);
        std::vector<ophidian::routing::GCell> expected_gcells;
        expected_gcells.push_back(gcell); //graph.gcell(2, 3, 1)
        expected_gcells.push_back(graph.gcell(2, 2, 1));
        CHECK(std::is_permutation(gcells.begin(), gcells.end(), expected_gcells.begin()));
    }
}