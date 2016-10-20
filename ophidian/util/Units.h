#ifndef OPHIDIAN_UTIL_UNITS_H
#define OPHIDIAN_UTIL_UNITS_H

#include <ratio>
#include <3rdparty/units/include/units.h>

namespace ophidian {
namespace util {

//Length Units
using picometer_t   = units::length::picometer_t;
using nanometer_t   = units::length::nanometer_t;
using micrometer_t  = units::length::micrometer_t;
using millimeter_t  = units::length::millimeter_t;
using meter_t       = units::length::meter_t;

//Area Units
using square_micrometer_t = units::unit<std::nano, units::category::length_unit>;
using square_millimeter_t = units::unit<std::micro, units::category::length_unit>;
using square_meter_t = units::area::square_meter_t;

//Time Units
using picosecond_t  = units::time::picosecond_t;
using nanosecond_t  = units::time::nanosecond_t;
using microsecond_t = units::time::microsecond_t;
using millisecond_t  = units::time::millisecond_t;
using second_t      = units::time::second_t;

//Capacitance Units
using femtofarad_t = units::capacitance::femtofarad_t;
using picofarad_t  = units::capacitance::picofarad_t;
using farad_t      = units::capacitance::farad_t;

//Resistance Units
using ohm_t       = units::impedance::ohm_t;
using kiloohm_t   = units::impedance::kiloohm_t;


} //namespace util

} //namespace ophidian

#endif // OPHIDIAN_UTIL_UNITS_H
