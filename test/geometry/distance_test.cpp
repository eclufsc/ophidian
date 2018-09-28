#include <catch.hpp>

#include <ophidian/geometry/Distance.h>
#include <ophidian/util/Units.h>

using namespace ophidian::geometry;

template <typename T>
void test_manhattan_distance(T&& x1, T&& y1, T&& x2, T&& y2, T&& result)
{
    auto point_1 = Point<T>(x1, y1);
    auto point_2 = Point<T>(x2, y2);

    CHECK(ManhattanDistance(point_1, point_2) == result);
}

TEST_CASE("Geometry: manhattan distance between two points", "[geometry][distance]")
{
    SECTION("Test distance from points of double")
    {
        test_manhattan_distance(0.0d, 0.0d, 0.0d, 0.0d, 0.0d);

        test_manhattan_distance(5.0d, 10.0d, 3.0d, 7.0d, 5.0d);

        test_manhattan_distance(-5.0d, -10.0d, -3.0d, -7.0d, 5.0d);
    }

    SECTION("Test distance from points of database_unit_t")
    {
        using dbu = ophidian::util::database_unit_t;

        test_manhattan_distance(dbu{0}, dbu{0}, dbu{0}, dbu{0}, dbu{0});

        test_manhattan_distance(dbu{5}, dbu{10}, dbu{3}, dbu{7}, dbu{5});

        test_manhattan_distance(dbu{-5}, dbu{-10}, dbu{-3}, dbu{-7}, dbu{5});
    }
}

template <typename T>
void test_euclidean_distance(T&& x1, T&& y1, T&& x2, T&& y2, T&& result)
{
    auto point_1 = Point<T>(x1, y1);
    auto point_2 = Point<T>(x2, y2);

    CHECK(EuclideanDistance(point_1, point_2) == result);
}

TEST_CASE("Geometry: euclidean distance between two points", "[geometry][distance]")
{
    SECTION("Test distance from points of double")
    {
        test_euclidean_distance(0.0d, 0.0d, 0.0d, 0.0d, 0.0d);

        test_euclidean_distance(5.0d, 10.0d, 3.0d, 7.0d, std::sqrt(13));

        test_euclidean_distance(-5.0d, -10.0d, -3.0d, -7.0d, std::sqrt(13));
    }
}
