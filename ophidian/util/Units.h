/*
 * Copyright 2017 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at
  http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#ifndef OPHIDIAN_UTIL_UNITS_H
#define OPHIDIAN_UTIL_UNITS_H

#include <ratio>
#include <vector>
#include <3rdparty/units/include/units.h>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry.hpp>
#include <boost/variant.hpp>

#include <ophidian/geometry/Models.h>

//! Declaration of basic units metrics types from 'units' third-party library. These units are typesafe and have automatic conversion for units belonging to the same family.
//! For example, the multiplication of capacitance and resistance unit gives a time unit.
namespace ophidian {
namespace util {

//!Length Units
using picometer_t   = units::length::picometer_t;
using nanometer_t   = units::length::nanometer_t;
using micrometer_t  = units::length::micrometer_t;
using millimeter_t  = units::length::millimeter_t;
using meter_t       = units::length::meter_t;

//!Lenght Unit derived from micrometer to be used as internal unit length unit (as commonly used in LEF/DEF library)
//!The base dbu/micron ratio is assumed to be 1/2000. Other ratios can be derived during runtime
using dbu_base = units::unit<std::ratio<1, 2000>, units::length::micrometers>;
using dbumeter_t = units::unit_t<dbu_base, double, units::linear_scale>;

using dbu_option_1 = units::unit<std::ratio<1, 1000>, units::length::micrometers>;
using dbu_option_2 = units::unit<std::ratio<1, 2000>, units::length::micrometers>;
using dbu_option_1_t = units::unit_t<dbu_option_1, double, units::linear_scale>;
using dbu_option_2_t = units::unit_t<dbu_option_2, double, units::linear_scale>;

using dbu_factor = boost::variant<dbu_option_1_t, dbu_option_2_t>;

class DbuSelector
{
public:
  static dbu_factor getDbuFactor(int factor, double value)
  {
    if(factor == 1000)
    {
      dbu_option_1_t val(value);
      return dbu_factor(val);
    }
    else
    {
      dbu_option_2_t val(value);
      return dbu_factor(val);
    }
  }
};

class DbuVisitor : public boost::static_visitor<dbumeter_t>
{
public:
    dbumeter_t operator()(dbu_option_1_t i) const
    {
      return dbumeter_t(i);
    }
    
    dbumeter_t operator()(dbu_option_2_t i) const
    {
      return dbumeter_t(i);
    }
};


//!Area Units
//!New unit tag derived from area_unit to allow for creation of square_millimiter_t
using square_millimeter = units::unit<std::micro, units::category::area_unit>;

using square_millimeter_t = units::unit_t<square_millimeter>;
using square_meter_t = units::area::square_meter_t;

//!Time Units
using picosecond_t  = units::time::picosecond_t;
using nanosecond_t  = units::time::nanosecond_t;
using microsecond_t = units::time::microsecond_t;
using millisecond_t  = units::time::millisecond_t;
using second_t      = units::time::second_t;

//!Capacitance Units
using femtofarad_t = units::capacitance::femtofarad_t;
using picofarad_t  = units::capacitance::picofarad_t;
using farad_t      = units::capacitance::farad_t;

//!Resistance Units
using ohm_t       = units::impedance::ohm_t;
using kiloohm_t   = units::impedance::kiloohm_t;

} //namespace util

} //namespace ophidian

//-----------------------------End of unit metrics---------------------------------//

namespace boost {
namespace geometry {
namespace model {
namespace d2 {

//!Creating a new boost point to allow receiving double in the constructor and converting to micrometer_t
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

    ///operator == overloading
    bool operator!=(const point_xy_location & point)
    { return !(*this==point);}
};

} //namespace d2
} //namespace model
} //namespace geometry
} //namespace boost


namespace ophidian {
namespace util {

using Location = boost::geometry::model::d2::point_xy_location<ophidian::util::micrometer_t>;

//!Class multibox using geometry::Box 
class MultiBox {
public:
    //!Standard constructor
    MultiBox() {

    }

    //!Constructor receiving a vector of geometry::Box
    MultiBox(const std::vector<geometry::Box> & boxes)
        : boxes_(boxes) {

    }

    //!Copy constructor
    MultiBox(const MultiBox & otherBox)
        : boxes_(otherBox.boxes_) {

    }

    //!Push back a geometry::Box
    void push_back(const geometry::Box & box) {
        boxes_.push_back(box);
    }

    //!Non-const iterator begin
    std::vector<geometry::Box>::iterator begin() {
        return boxes_.begin();
    }

    //!Non-const iterator end
    std::vector<geometry::Box>::iterator end() {
        return boxes_.end();
    }

    //!Const iterator begin
    std::vector<geometry::Box>::const_iterator begin() const {
        return boxes_.begin();
    }

    //!Const iterator end
    std::vector<geometry::Box>::const_iterator end() const {
        return boxes_.end();
    }

    //!Operator overloading for comparison of two multibox objects
    bool operator==(const MultiBox & other) const {
        for (auto box1 : this->boxes_) {
            for (auto box2 : other.boxes_) {
                bool comparison = (box1.min_corner().x() == box2.min_corner().x()) && (box1.min_corner().y() == box2.min_corner().y())
                        && (box1.max_corner().x() == box2.max_corner().x()) && (box1.max_corner().y() == box2.max_corner().y());
                if (!comparison) {
                    return false;
                }
            }
        }
        return true;
    }

    //!Operator overload for difference between two multibox objects
    bool operator!=(const MultiBox & other) const {
        return !(*this==other);
    }

private:
    std::vector<geometry::Box> boxes_;
};

} //namespace util
} //namespace ophidian

#endif // OPHIDIAN_UTIL_UNITS_H
