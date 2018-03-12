#include <catch.hpp>
#include <ophidian/util/Units.h>

using namespace ophidian::util;

TEST_CASE("Units Test: Length Units", "[util][units]")
{
  nanometer_t value_nm = 20.5_nm;
  picometer_t value_pm = 2000.0_pm;
  nanometer_t result_nm = value_pm + value_nm;
  REQUIRE(result_nm == nanometer_t(22.5));

  millimeter_t value_mm(10.2);
  micrometer_t value_um(5000);
  micrometer_t result_um = value_mm + value_um;
  REQUIRE(result_um == micrometer_t(15200));
}

TEST_CASE("Units Test: DBU to um converter", "[util][units]")
{
  int x_lef = 200;
  int y_lef = 100;
  dbumeter_t x_dbu(x_lef);
  dbumeter_t y_dbu(y_lef);

  DbuConverter dbuConverter_1(1000);
  micrometer_t x_um_1 = dbuConverter_1.convert(x_dbu);
  micrometer_t y_um_1 = dbuConverter_1.convert(y_dbu);
  REQUIRE(x_um_1 == micrometer_t(0.2));
  REQUIRE(y_um_1 == micrometer_t(0.1));
  dbumeter_t x_dbu_1 = dbuConverter_1.convert(x_um_1);
  dbumeter_t y_dbu_1 = dbuConverter_1.convert(y_um_1);
  REQUIRE(x_dbu_1 == x_dbu);
  REQUIRE(y_dbu_1 == y_dbu);

  DbuConverter dbuConverter_2(2000);
  micrometer_t x_um_2 = dbuConverter_2.convert(x_dbu);
  micrometer_t y_um_2 = dbuConverter_2.convert(y_dbu);
  REQUIRE(x_um_2 == micrometer_t(0.1));
  REQUIRE(y_um_2 == micrometer_t(0.05));
  dbumeter_t x_dbu_2 = dbuConverter_2.convert(x_um_2);
  dbumeter_t y_dbu_2 = dbuConverter_2.convert(y_um_2);
  REQUIRE(x_dbu_2 == x_dbu);
  REQUIRE(y_dbu_2 == y_dbu);

  DbuConverter dbuConverter_3(4000);
  micrometer_t x_um_3 = dbuConverter_3.convert(x_dbu);
  micrometer_t y_um_3 = dbuConverter_3.convert(y_dbu);
  REQUIRE(x_um_3 == micrometer_t(0.05));
  REQUIRE(y_um_3 == micrometer_t(0.025));
  dbumeter_t x_dbu_3 = dbuConverter_3.convert(x_um_3);
  dbumeter_t y_dbu_3 = dbuConverter_3.convert(y_um_3);
  REQUIRE(x_dbu_3 == x_dbu);
  REQUIRE(y_dbu_3 == y_dbu);
}

TEST_CASE("Units Test: Area", "[util][units]")
{
  square_meter_t value_sqm(1.5);
  square_millimeter_t value_sqmm(1e+6);
  square_meter_t result_sqm = value_sqm + value_sqmm;
  REQUIRE(result_sqm == square_meter_t(2.5));

  square_millimeter_t value_sqmm_2(1000.0);
  square_millimeter_t value_sqmm_3(5000.0);
  square_meter_t result_sqm_2 = value_sqmm_2 + value_sqmm_3;
  REQUIRE(result_sqm_2 == square_meter_t(6e-3));
}

TEST_CASE("Units Test: Time", "[util][units]")
{
    second_t value_s = 10.2_s;
    millisecond_t value_ms = 1000_ms;
    second_t result = value_s + value_ms;
    REQUIRE(result == second_t(11.2));

    microsecond_t result_2 = value_s + value_ms;
    REQUIRE(result_2 == microsecond_t(11.2e+6));
}

TEST_CASE("Units Test: Capacitance Units", "[util][units]")
{
  femtofarad_t value_ff(25.5);
  picofarad_t value_pf(0.02);
  femtofarad_t result_ff = value_pf + value_ff;
  picofarad_t result_pf = value_pf + value_ff;
  REQUIRE(result_ff == femtofarad_t(45.5));
  REQUIRE(result_pf == picofarad_t(0.0455));
}

TEST_CASE("Units Test: Resistance Units", "[util][units]")
{
  kiloohm_t value_ko(35.5);
  ohm_t value_o(1200);
  kiloohm_t result_ko = value_ko + value_o;
  ohm_t result_o = value_ko + value_o;
  REQUIRE(result_ko == kiloohm_t(36.7));
  REQUIRE(result_o == ohm_t(36.7e+3));
}

TEST_CASE("Unit Test: time = Cap x Res", "[util][units]")
{
    kiloohm_t value_ko(10.0);
    femtofarad_t value_ff(2);
    picosecond_t result_ps = value_ko * value_ff;
    REQUIRE(result_ps == picosecond_t(20.0));

    kiloohm_t value_ko_2(25.0);
    picofarad_t value_pf(20.0);
    nanosecond_t result_ns = value_ko_2 * value_pf;
    REQUIRE(result_ns == nanosecond_t(500.0));
}

TEST_CASE("Location Test", "[util][units]")
{
    LocationMicron loc_um(10.0, 20.0);
    LocationMicron loc_nm(nanometer_t(1000.0), nanometer_t(2000.0));

    LocationMicron loc_result(loc_um.x() + loc_nm.x(), loc_um.y() + loc_nm.y());
    LocationMicron loc_result_golden(11.0, 22.0);
    REQUIRE(loc_result.x() == loc_result_golden.x());
    REQUIRE(loc_result.y() == loc_result_golden.y());

    DbuConverter dbuConverter_1(1000);
    LocationDbu loc_dbu = dbuConverter_1.convert(loc_um);
    REQUIRE(loc_dbu == LocationDbu(10000.0, 20000.0));
    LocationMicron loc_um_conv = dbuConverter_1.convert(loc_dbu);
    REQUIRE(loc_um == loc_um_conv);
}
