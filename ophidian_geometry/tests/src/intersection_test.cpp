#include <catch2/catch.hpp>

#include <ophidian/util/Units.h>
#include <ophidian/geometry/Models.h>
#include <ophidian/geometry/Operations.h>

using namespace ophidian::geometry;

TEST_CASE("Geometry: intersection between two boxes", "[geometry][operations]") {
    SECTION("Intersection betwen box of double")
    {
        auto box1 = Box<double>{{1.0, 1.0}, {3.0, 3.0}};
        auto box2 = Box<double>{{2.0, 1.0}, {4.0, 3.0}};

        auto result_box = Box<double>{};

        intersection(box1, box2, result_box);

        REQUIRE(result_box.min_corner().x() == Approx(2.0));
        REQUIRE(result_box.min_corner().y() == Approx(1.0));
        REQUIRE(result_box.max_corner().x() == Approx(3.0));
        REQUIRE(result_box.max_corner().y() == Approx(3.0));
    }

    SECTION("Intersection betwen box of database_unit_t")
    {
        using dbu = ophidian::util::database_unit_t;

        auto box1 = Box<dbu>{{dbu{1.0}, dbu{1.0}}, {dbu{3.0}, dbu{3.0}}};
        auto box2 = Box<dbu>{{dbu{2.0}, dbu{1.0}}, {dbu{4.0}, dbu{3.0}}};

        auto result_box = Box<dbu>{};

        intersection(box1, box2, result_box);

        REQUIRE(result_box.min_corner().x() == dbu{2.0});
        REQUIRE(result_box.min_corner().y() == dbu{1.0});
        REQUIRE(result_box.max_corner().x() == dbu{3.0});
        REQUIRE(result_box.max_corner().y() == dbu{3.0});
    }
}
