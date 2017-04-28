#include <catch.hpp>
#include <ophidian/util/Units.h>

using namespace ophidian::util;


TEST_CASE("Units Test: Length", "[util][units]")
{
  micrometer_t mm1(0.015);
  micrometer_t mm2(0.025);
  dbumeter_t result_1 = mm1 + mm2;
  dbumeter_t golden_result_1(80);
  REQUIRE(result_1 == golden_result_1);
}

TEST_CASE("Units Test: DBU", "[util][units]")
{
  double x = 200;
  double y = 150;

  dbu_factor unit_x = DbuSelector::getDbuFactor(1000, x);
  dbu_factor unit_y = DbuSelector::getDbuFactor(1000, y);

  dbumeter_t x_dbu = boost::apply_visitor( DbuVisitor(), unit_x);
  dbumeter_t y_dbu = boost::apply_visitor( DbuVisitor(), unit_y);

  dbumeter_t x_dbu_golden_1(400);
  dbumeter_t y_dbu_golden_1(300);
  REQUIRE(x_dbu == x_dbu_golden_1);
  REQUIRE(y_dbu == y_dbu_golden_1);

  unit_x = DbuSelector::getDbuFactor(2000, x);
  unit_y = DbuSelector::getDbuFactor(2000, y);

  x_dbu = boost::apply_visitor( DbuVisitor(), unit_x);
  y_dbu = boost::apply_visitor( DbuVisitor(), unit_y);

  dbumeter_t x_dbu_golden_2(200);
  dbumeter_t y_dbu_golden_2(150);
  REQUIRE(x_dbu == x_dbu_golden_2);
  REQUIRE(y_dbu == y_dbu_golden_2);
}

TEST_CASE("Units Test: Area", "[util][units]")
{
  square_meter_t m1(1.5);
  square_millimeter_t mm1(1e+6);
  square_meter_t result_1 = m1 + mm1;
  square_meter_t golden_result_1(2.5);
  REQUIRE(result_1 == golden_result_1);

  square_millimeter_t mm2(1000.0);
  square_millimeter_t mm3(5000.0);
  square_meter_t result_2 = mm2 + mm3;
  square_meter_t golden_result_2(6e-3);
  REQUIRE(result_2 == golden_result_2);
}

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
    Location loc1micron(10.0, 20.0);
    Location loc2nano(nanometer_t(1000.0), nanometer_t(2000.0));

    Location loc3micron(loc1micron.x() + loc2nano.x(), loc1micron.y() + loc2nano.y());
    Location loc3micronGoldenResult(11.0, 22.0);
    REQUIRE(loc3micron.x() == loc3micronGoldenResult.x());
    REQUIRE(loc3micron.y() == loc3micronGoldenResult.y());

}
