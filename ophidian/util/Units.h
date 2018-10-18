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

// std headers
#include <ratio>
#include <vector>

// external headers
#include <units/units.h>

// external headers
#include <ophidian/geometry/Models.h>

namespace ophidian::util
{
    //!Lenght Unit derived from micrometer to be used as internal length unit length (as commonly used in LEF/DEF library)
    //!To convert to/from micron, please use the DbuConverter Utility class
    using database_unit_base = units::unit<std::ratio<1>, units::length::micrometers>;
    using database_unit_t = units::unit_t<database_unit_base, double, units::linear_scale>;

    using database_unit_scalar_base = units::unit<std::ratio<1>, units::category::scalar_unit>;
    using database_unit_scalar_t = units::unit_t<database_unit_scalar_base, double,
        units::linear_scale>;

    //! boost bidimensional point for the unit micrometer_t
    using LocationMicron = geometry::Point<units::length::micrometer_t>;

    //! boost bidimensional point for the unit database_unit_t
    using LocationDbu = geometry::Point<database_unit_t>;

    using picometer_t = units::length::picometer_t;
    using nanometer_t = units::length::nanometer_t;
    using micrometer_t = units::length::micrometer_t;
    using millimeter_t = units::length::millimeter_t;
    using meter_t = units::length::meter_t;

    //!Area Units
    //!New unit tag derived from area_unit to allow for creation of square_millimiter_t
    using square_millimeter = units::unit <std::micro, units::category::area_unit>;

    using square_millimeter_t = units::unit_t <square_millimeter>;
    using square_meter_t = units::area::square_meter_t;

    //!Time Units
    using picosecond_t = units::time::picosecond_t;
    using nanosecond_t = units::time::nanosecond_t;
    using microsecond_t = units::time::microsecond_t;
    using millisecond_t = units::time::millisecond_t;
    using second_t = units::time::second_t;

    //!Capacitance Units
    using femtofarad_t = units::capacitance::femtofarad_t;
    using picofarad_t = units::capacitance::picofarad_t;
    using nanofarad_t = units::capacitance::nanofarad_t;
    using farad_t = units::capacitance::farad_t;

    //!Resistance Units
    using ohm_t = units::impedance::ohm_t;
    using kiloohm_t = units::impedance::kiloohm_t;


    //!Class to handle the conversion from micron <-> DBU. It has to be instantiated by a def/lef library, which defines the DBU to micron factor
    class DbuConverter
    {
    public:
        DbuConverter(const DbuConverter& other) = default;
        DbuConverter(DbuConverter&& other) = default;

        DbuConverter(const database_unit_scalar_t& dbu_factor):
            m_dbu_factor{dbu_factor}
        {}

        //!Converts a value from micrometer_t to database_unit_t
        database_unit_t convert(micrometer_t value) const
        {
            return database_unit_t(value * m_dbu_factor);
        }

        //!Converts a value from database_unit_t to micrometer_t
        micrometer_t convert(database_unit_t value) const
        {
            return micrometer_t(value / m_dbu_factor);
        }

        //!Converts a LocationMicron to LocationDbu
        LocationDbu convert(LocationMicron value) const
        {
            database_unit_t conv_value_x = value.x() * m_dbu_factor;
            database_unit_t conv_value_y = value.y() * m_dbu_factor;

            return LocationDbu{conv_value_x, conv_value_y};
        }

        //!Converts a LocationDbu to LocationMicron
        LocationMicron convert(LocationDbu value) const
        {
            micrometer_t conv_value_x = value.x() / m_dbu_factor;
            micrometer_t conv_value_y = value.y() / m_dbu_factor;

            return LocationMicron{conv_value_x, conv_value_y};
        }

    private:
        const database_unit_scalar_t m_dbu_factor;
    };
}     //namespace ophidian

#endif // OPHIDIAN_UTIL_UNITS_H
