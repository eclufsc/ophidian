/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
*/


#include "../catch.hpp"

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/io.hpp>

#include "../timing/lookup_table.h"

TEST_CASE("lookup table/create with size", "[timing][lut]") {

	ophidian::timing::lookup_table<double, double, double> table(1, 1);

	REQUIRE(table.row_count() == 1);
	REQUIRE(table.column_count() == 1);

	REQUIRE_THROWS(table.at(1, 1));
	REQUIRE_NOTHROW(table.at(0, 0));

}

TEST_CASE("lookup table/set", "[timing][lut]") {
	ophidian::timing::lookup_table<double, double, double> table(1, 2);
	table.at(0, 0, 1.0);
	table.at(0, 1, 2.0);

	REQUIRE(table.at(0, 0) == 1.0);
	REQUIRE(table.at(0, 1) == 2.0);
}

TEST_CASE("lookup table/set row values", "[timing][lut]") {
	ophidian::timing::lookup_table<double, double, double> table(1, 2);
	table.row_value(0, 10.0);
	REQUIRE(table.row_value(0) == 10.0);
}

TEST_CASE("lookup table/set column values", "[timing][lut]") {
	ophidian::timing::lookup_table<double, double, double> table(1, 2);
	table.column_value(0, 10.0);
	table.column_value(1, 30.0);
	REQUIRE(table.column_value(0) == 10.0);
	REQUIRE(table.column_value(1) == 30.0);
}

TEST_CASE("lookup table/interpolation", "[timing][lut]") {
	using namespace boost::units;
	ophidian::timing::lookup_table<quantity<si::capacitance>, quantity<si::time>, quantity<si::time> > table(2, 2);
	table.row_value(0, quantity<si::capacitance>(0.0 * si::femto * si::farads));
	table.row_value(1, quantity<si::capacitance>(6.0 * si::femto * si::farads));
	table.column_value(0, quantity<si::time>(0.0 * si::pico * si::seconds));
	table.column_value(1, quantity<si::time>(5.0 * si::pico * si::seconds));

	table.at(0, 0, quantity<si::time>(5.208 * si::pico * si::seconds));
	table.at(0, 1, quantity<si::time>(9.792 * si::pico * si::seconds));
	table.at(1, 0, quantity<si::time>(9.376 * si::pico * si::seconds));
	table.at(1, 1, quantity<si::time>(14.576 * si::pico * si::seconds));

	quantity<si::capacitance> row_value = table.row_value(1);
	quantity<si::time> column_value = table.column_value(0);
	REQUIRE(table.compute(row_value, column_value) == table.at(1, 0));

	auto result = table.compute(quantity<si::capacitance>(5.5 * si::femto * si::farads), quantity<si::time>(4.7 * si::pico * si::seconds));
	REQUIRE((result - quantity<si::time>(1.38684e-11 * si::seconds)) <= quantity<si::time>(1e-4 * si::pico * si::seconds));

}

