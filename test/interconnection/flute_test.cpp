#include "flute_test.h"
#include <catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/geometry/Models.h>

using namespace ophidian;

TEST_CASE("Flute singleton", "[interconnection]")
{
    REQUIRE_NOTHROW( interconnection::Flute::instance() );
}

TEST_CASE("Flute tree with one point", "[interconnection]")
{
    interconnection::Flute & flute = interconnection::Flute::instance();
    std::unique_ptr<interconnection::SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}});
    REQUIRE( tree->numPoints() == 1 );
    REQUIRE( tree->numSegments() == 0 );

}


TEST_CASE("Flute tree with two point", "[interconnection]")
{
    interconnection::Flute & flute = interconnection::Flute::instance();
    std::unique_ptr<interconnection::SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}, {4.0, 4.0}});
    REQUIRE( tree->numPoints() == 2 );
    REQUIRE( tree->numSegments() == 1 );
}

TEST_CASE("Flute tree with three points", "[interconnection]")
{
    interconnection::Flute & flute = interconnection::Flute::instance();
    std::unique_ptr<interconnection::SteinerTree> tree = flute.create(std::vector<geometry::Point>{{2.0, 2.0}, {2.0, 4.0}, {4.0, 4.0}});
    REQUIRE( tree->numPoints() == 3 );
    REQUIRE( tree->numSegments() == 2 );
}
