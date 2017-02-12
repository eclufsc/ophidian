#ifndef OPHIDIAN_UTIL_UNITS_H
#define OPHIDIAN_UTIL_UNITS_H

#include <ratio>
#include <3rdparty/units/include/units.h>
#include <boost/geometry/geometries/point_xy.hpp>

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


//Creating a new boost point to allow receiving double in the constructor and converting to micrometer_t
namespace boost {
namespace geometry {
namespace model {
namespace d2 {
template<typename CoordinateType, typename CoordinateSystem = cs::cartesian>
class point_xy_location : public model::point<CoordinateType, 2, CoordinateSystem>
{
public:

#ifndef BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
    /// \constructor_default_no_init
    point_xy_location() = default;
#else
    /// \constructor_default_no_init
    inline point_xy()
    {}
#endif

    /// Constructor with x/y values as double with implicit convertion to micrometer_t
    inline point_xy_location(double const& x, double const& y)
        : model::point<CoordinateType, 2, CoordinateSystem>(CoordinateType(x), CoordinateType(y))
    {}

    /// Constructor with x/y values
    inline point_xy_location(CoordinateType const& x, CoordinateType const& y)
        : model::point<CoordinateType, 2, CoordinateSystem>(x, y)
    {}

    /// Get x-value
    inline CoordinateType const& x() const
    { return this->template get<0>(); }

    /// Get y-value
    inline CoordinateType const& y() const
    { return this->template get<1>(); }

    /// Set x-value
    inline void x(CoordinateType const& v)
    { this->template set<0>(v); }

    /// Set y-value
    inline void y(CoordinateType const& v)
    { this->template set<1>(v); }

    ///operator == overloading
    bool operator==(const point_xy_location & point)
    { return (this->x() == point.x()) && (this->y() == point.y());}
};
}
}
}
}

namespace ophidian {
namespace util {

using Location = boost::geometry::model::d2::point_xy_location<ophidian::util::micrometer_t>;

} //namespace util

} //namespace ophidian

#endif // OPHIDIAN_UTIL_UNITS_H
