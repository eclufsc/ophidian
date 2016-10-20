#include <catch.hpp>
#include <ophidian/util/Units.h>
#include <ophidian/geometry/Models.h>


using namespace ophidian::util;

TEST_CASE("Units Test: Second", "[util][units]")
{
    second_t a(10.2);
    millisecond_t b(1000);
    second_t result = a + b;
    second_t goldenResult(11.2);
    REQUIRE(result == goldenResult);

    microsecond_t result2 = a + b;
    microsecond_t goldenResult2(11200000);
    REQUIRE(result == goldenResult);
}

TEST_CASE("Unit Test: Resistance", "[util][units]")
{
    kiloohm_t res1(10.0);
    femtofarad_t cap1(2);
    picosecond_t time1 = res1 * cap1;
    picosecond_t timeReference(20.0);
    REQUIRE(time1 == timeReference);
}


TEST_CASE("Location Test", "[util][units]")
{
    ophidian::geometry::Location loc1micron(micrometer_t(10.0), micrometer_t(20.0));
    ophidian::geometry::Location loc2nano(nanometer_t(1000.0), nanometer_t(2000.0));

    ophidian::geometry::Location loc3micron(loc1micron.x() + loc2nano.x(), loc1micron.y() + loc2nano.y());
    ophidian::geometry::Location loc3micronGoldenResult(micrometer_t(11.0), micrometer_t(22.0));
    REQUIRE(loc3micron.x() == loc3micronGoldenResult.x());
    REQUIRE(loc3micron.y() == loc3micronGoldenResult.y());

}
