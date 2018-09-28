#include <catch.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

template <class T>
void test_point_rotation(Point<T>&& point, double&& degree, Point<T>&& result)
{
    auto rotated = rotate(point, degree);

    CHECK(rotated.x() == Approx(result.x()) );
    CHECK(rotated.y() == Approx(result.y()) );
}

TEST_CASE("Geometry: rotating a Point", "[geometry][operations]")
{
    SECTION("Rotate point of double")
    {
        test_point_rotation<double>({0.0d, 0.0d}, 90.0d, {0.0d, 0.0d});

        test_point_rotation<double>({1.0d, 2.0d}, 90.0d, {2.0d, -1.0d});

        test_point_rotation<double>({1.0d, 2.0d}, 180.0d, {-1.0d, -2.0d});
    }
}

template <class T>
void test_box_rotation(Box<T>&& box, double&& degree, Box<T>&& result)
{
    auto rotated = rotate(box, degree);

    CHECK(rotated.min_corner().x() == Approx(result.min_corner().x()) );
    CHECK(rotated.min_corner().y() == Approx(result.min_corner().y()) );
    CHECK(rotated.max_corner().x() == Approx(result.max_corner().x()) );
    CHECK(rotated.max_corner().y() == Approx(result.max_corner().y()) );
}

TEST_CASE("Geometry: rotating a Box", "[geometry][operations]")
{
    SECTION("Rotate box of double")
    {
        test_box_rotation<double>(
            {{1.0d, 2.0d}, {2.0d, 2.0d}},
            0.0d,
            {{1.0d, 2.0d}, {2.0d, 2.0d}}
        );

        test_box_rotation<double>(
            {{1.0d, 2.0d}, {2.0d, 2.0d}},
            90.0d,
            {{2.0d, -2.0d}, {2.0d, -1.0d}}
        );
    }
}
