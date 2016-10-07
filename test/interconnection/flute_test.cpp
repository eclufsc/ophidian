#include "flute_test.h"
#include <catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Distance.h>

using namespace ophidian::interconnection;
using namespace ophidian;

TEST_CASE("Flute singleton", "[interconnection]")
{
    REQUIRE_NOTHROW( Flute::instance() );
}

TEST_CASE("Flute tree with one point", "[interconnection]")
{
    Flute & flute = Flute::instance();
    std::unique_ptr<SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}});
    REQUIRE( tree->size(SteinerTree::Point{}) == 1 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 0 );

}


TEST_CASE("Flute tree with two point", "[interconnection]")
{
    Flute & flute = Flute::instance();
    std::unique_ptr<SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}, {4.0, 4.0}});
    REQUIRE( tree->size(SteinerTree::Point{}) == 2 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 1 );
}


TEST_CASE("Flute tree with three points", "[interconnection]")
{
    Flute & flute = Flute::instance();
    std::unique_ptr<SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}, {2.0, 4.0}, {4.0, 4.0}});
    REQUIRE( tree->size(SteinerTree::Point{}) == 3 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 2 );
    geometry::ManhattanDistance distance;
    REQUIRE( Approx(tree->length()) == distance({2.0, 2.0}, {4.0, 4.0}) );
}


TEST_CASE("Flute with negative coordinates", "[interconnection]")
{
    Flute & flute = Flute::instance();
    auto const tree1 = flute.create(std::vector<geometry::Point>{{-2.0, -2.0}, {-2.0, -4.0}, {-4.0, -4.0}, {-13.0, -13.0}});
    auto const tree2 = flute.create(std::vector<geometry::Point>{{2.0, 2.0}, {2.0, 4.0}, {4.0, 4.0}, {13.0, 13.0}});
    REQUIRE( Approx(tree1->length()) == tree2->length() );
}
