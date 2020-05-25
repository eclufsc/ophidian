#include <catch2/catch.hpp>
#include <ophidian/interconnection/SteinerTree.h>
#include <ophidian/interconnection/Flute.h>
#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Distance.h>
#include <lemon/graph_to_eps.h>

using namespace ophidian::interconnection;
using namespace ophidian;

TEST_CASE("Flute singleton", "[interconnection]")
{
    REQUIRE_NOTHROW( Flute::instance() );
}

TEST_CASE("Flute tree with one point", "[interconnection]")
{
    auto& flute = Flute::instance();
    auto tree = flute.create(std::vector{Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{2.0}}});

    REQUIRE( tree->size(SteinerTree::Point{}) == 1 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 0 );
}

TEST_CASE("Flute tree with two point", "[interconnection]")
{
    auto& flute = Flute::instance();
    auto tree = flute.create(
        std::vector{
            Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{2.0}},
            Flute::Point{Flute::dbu_t{4.0}, Flute::dbu_t{4.0}}
        }
    );
    REQUIRE( tree->size(SteinerTree::Point{}) == 2 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 1 );
}

TEST_CASE("Flute tree with three points", "[interconnection]")
{
    auto& flute = Flute::instance();
    auto tree = flute.create(
        std::vector{
            Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{2.0}},
            Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{4.0}},
            Flute::Point{Flute::dbu_t{4.0}, Flute::dbu_t{4.0}}
        }
    );

    REQUIRE( tree->size(SteinerTree::Point{}) == 3 );
    REQUIRE( tree->size(SteinerTree::Segment{}) == 2 );
    REQUIRE( Approx(tree->length()) == geometry::ManhattanDistance(geometry::Point<double>{2.0, 2.0}, geometry::Point<double>{4.0, 4.0}) );
}

#include <lemon/smart_graph.h>

class Repeat {
public:
    template <uint32_t N, class Functor>
    static bool run(Functor func)
    {
        for(uint32_t i = 0; i < N; ++i)
        {
            if(!func(i))
            {
                return false;
            }
        }
        return true;
    }
};

#include <ophidian/interconnection/ToEps.h>

TEST_CASE("Flute with negative coordinates", "[interconnection]")
{
    auto& flute = Flute::instance();

    auto const tree1 = flute.create(
        std::vector{
            Flute::Point{Flute::dbu_t{-2.0}, Flute::dbu_t{-2.0}},
            Flute::Point{Flute::dbu_t{-2.0}, Flute::dbu_t{-4.0}},
            Flute::Point{Flute::dbu_t{-4.0}, Flute::dbu_t{-4.0}},
            Flute::Point{Flute::dbu_t{-13.0}, Flute::dbu_t{-13.0}}
        }
    );

    auto const tree2 = flute.create(
        std::vector{
            Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{2.0}},
            Flute::Point{Flute::dbu_t{2.0}, Flute::dbu_t{4.0}},
            Flute::Point{Flute::dbu_t{4.0}, Flute::dbu_t{4.0}},
            Flute::Point{Flute::dbu_t{13.0}, Flute::dbu_t{13.0}}
        }
    );

    REQUIRE( Approx(tree1->length()) == tree2->length() );
}

TEST_CASE("Flute to EPS", "[interconnection]")
{
    auto& flute = Flute::instance();
    auto const tree = flute.create(
        std::vector{
            Flute::Point{Flute::dbu_t{0.0}, Flute::dbu_t{0.0}},
            Flute::Point{Flute::dbu_t{50.0}, Flute::dbu_t{4.0}},
            Flute::Point{Flute::dbu_t{15.0}, Flute::dbu_t{-30.0}},
            Flute::Point{Flute::dbu_t{43.0}, Flute::dbu_t{22.0}}
        }
    );

    ToEps::run(*tree, "output.eps");
}
