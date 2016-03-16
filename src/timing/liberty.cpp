/*
 * liberty.cpp
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#include "liberty.h"
#include <si2dr_liberty.h>
#include <cassert>
#include <iostream>

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

namespace openeda {
namespace timing {
namespace liberty {

si2drErrorT err;

namespace parser {
void read_pin(entity::entity cell_entity, si2drGroupIdT pin, library& library,
		boost::units::quantity<boost::units::si::time> time_unit,
		boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	si2drNamesIdT current_cell_group_names = si2drGroupGetNames(pin, &err);
	std::string pin_name { si2drIterNextName(current_cell_group_names, &err) };
	si2drIterQuit(current_cell_group_names, &err);

	auto pin_entity = library.std_cells().pin_create(cell_entity, pin_name);

	si2drAttrsIdT attrs = si2drGroupGetAttrs(pin, &err);
	si2drAttrIdT attr;

	while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {
		std::string attr_name { si2drAttrGetName(attr, &err) };
		//		if (attr_name == "direction")
		//			direction = si2drSimpleAttrGetStringValue(attr, &err);
		//		else if (attr_name == "max_capacitance")
		//			max_capacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
		if (attr_name == "capacitance")
			library.pin_capacitance(pin_entity, si2drSimpleAttrGetFloat64Value(attr, &err)*capacitive_load_unit);
		//		else if (attr_name == "clock")
		//			is_clock = si2drSimpleAttrGetBooleanValue(attr, &err);
	}
	si2drIterQuit(attrs, &err);

	//    std::cout << "    direction: " << direction << std::endl;
	//    std::cout << "    max_capacitance: " << max_capacitance << std::endl;
	//    std::cout << "    capacitance: " << pin_capacitance << std::endl;

	//	cell_pin.isInput = (direction == "input");
	//    cell_pin.maxCapacitance = max_capacitance*capacitive_unit_;
	//    cell_pin.capacitance = pin_capacitance*capacitive_unit_;
//	cell_pin.maxCapacitance = max_capacitance;
//	cell_pin.capacitance = pin_capacitance;
//	cell_pin.isClock = is_clock;
//	if (is_clock)
//		cell_type.isSequential = true;
//
//	si2drGroupsIdT groups = si2drGroupGetGroups(pin, &err_);
//	si2drGroupIdT group;
//	while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err_)),
//			&err_)) {
//		std::string group_type { si2drGroupGetGroupType(group, &err_) };
//		if (group_type == "timing") {
//			Liberty_Library_Timing_Info timing_info;
//			timing_info.toPin = cell_pin.name;
//			__read_timing(group, timing_info);
//			cell_type.timingArcs.push_back(timing_info);
//		}
//	}
//	si2drIterQuit(groups, &err_);
}

void read_cell(si2drGroupIdT cell, timing::library& library,
		boost::units::quantity<boost::units::si::time> time_unit,
		boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	si2drNamesIdT cell_names = si2drGroupGetNames(cell, &err);
	std::string name { si2drIterNextName(cell_names, &err) };
	si2drIterQuit(cell_names, &err);

	auto cell_entity = library.std_cells().cell_create(name);

	si2drGroupsIdT groups = si2drGroupGetGroups(cell, &err);
	si2drGroupIdT current_cell_group;

	while (!si2drObjectIsNull(
			(current_cell_group = si2drIterNextGroup(groups, &err)), &err)) {
		std::string current_cell_group_type { si2drGroupGetGroupType(
				current_cell_group, &err) };
		if (current_cell_group_type == "pin") {
			read_pin(cell_entity, current_cell_group, library, time_unit, capacitive_load_unit);
		}
	}

	//    std::cout << "  flipflop: " << (cell_type.isSequential?"true":"false") << std::endl;

	si2drIterQuit(groups, &err);

}
}

void read(std::string filename, timing::library& library) {
	si2drPIInit(&err);
	si2drReadLibertyFile(const_cast<char*>(filename.c_str()), &err);
	if (err == SI2DR_INVALID_NAME) {
		std::cout << "COULD NOT OPEN " << filename
				<< " for parsing-- quitting...\n";
		assert(false);
	} else if (err == SI2DR_SYNTAX_ERROR) {
		std::cout << "\nSyntax Errors were detected in the input file!\n\n";
		assert(false);
	}
	si2drGroupsIdT groups = si2drPIGetGroups(&err);
	si2drGroupIdT group;
	while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
		si2drCheckLibertyLibrary(group, &err);
		if (err == SI2DR_NO_ERROR) {
		} else
			assert(false);
	}
	si2drIterQuit(groups, &err);
	groups = si2drPIGetGroups(&err);
	while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
		si2drNamesIdT gnames;
		si2drStringT gname;
		si2drGroupsIdT library_groups;
		si2drGroupIdT current_group;
		gnames = si2drGroupGetNames(group, &err);
		gname = si2drIterNextName(gnames, &err);
		si2drIterQuit(gnames, &err);
		std::cout << "Liberty Library Name: " << (gname ? gname : "<NONAME>")
				<< std::endl;
		library_groups = si2drGroupGetGroups(group, &err);
		si2drAttrsIdT lib_attrs = si2drGroupGetAttrs(group, &err);
		si2drAttrIdT lib_attr;

		boost::units::quantity<boost::units::si::time> time_unit;
		boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit;

		while (!si2drObjectIsNull(
				(lib_attr = si2drIterNextAttr(lib_attrs, &err)), &err)) {
			std::string attr_name { si2drAttrGetName(lib_attr, &err) };
			if (attr_name == "time_unit") {
				std::string time_unit_string = si2drSimpleAttrGetStringValue(
						lib_attr, &err);
				if (time_unit_string == "1ps")
					time_unit = boost::units::quantity<boost::units::si::time>(
							1.0 * boost::units::si::pico
									* boost::units::si::second);
				else if (time_unit_string == "1ns")
					time_unit = boost::units::quantity<boost::units::si::time>(
							1.0 * boost::units::si::nano
									* boost::units::si::second);
				else if (time_unit_string == "1us")
					time_unit = boost::units::quantity<boost::units::si::time>(
							1.0 * boost::units::si::micro
									* boost::units::si::second);
				else if (time_unit_string == "1ms")
					time_unit = boost::units::quantity<boost::units::si::time>(
							1.0 * boost::units::si::milli
									* boost::units::si::second);
				else if (time_unit_string == "1s")
					time_unit = boost::units::quantity<boost::units::si::time>(
							1.0 * boost::units::si::second);
				else
					assert(false);
			} else if (attr_name == "capacitive_load_unit") {
				si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr,
						&err);
				si2drValueTypeT type;
				si2drInt32T int_val;
				si2drFloat64T double_val;
				si2drStringT string_val;
				si2drBooleanT bool_val;
				si2drExprT *expr;

				si2drIterNextComplexValue(values, &type, &int_val, &double_val,
						&string_val, &bool_val, &expr, &err);
				auto int_value = int_val;
				si2drIterNextComplexValue(values, &type, &int_val, &double_val,
						&string_val, &bool_val, &expr, &err);
				std::string str_value { string_val };
				si2drIterQuit(values, &err);

				if (str_value == "ff")
					capacitive_load_unit = boost::units::quantity<
							boost::units::si::capacitance>(
							int_value * boost::units::si::femto
									* boost::units::si::farads);
				else if (str_value == "pf")
					capacitive_load_unit = boost::units::quantity<
							boost::units::si::capacitance>(
							int_value * boost::units::si::pico
									* boost::units::si::farads);
				else if (str_value == "nf")
					capacitive_load_unit = boost::units::quantity<
							boost::units::si::capacitance>(
							int_value * boost::units::si::nano
									* boost::units::si::farads);
				else
					assert(false);
			}

		}

		si2drIterQuit(lib_attrs, &err);
		while (!si2drObjectIsNull(
				(current_group = si2drIterNextGroup(library_groups, &err)),
				&err)) {
			si2drStringT current_group_type = si2drGroupGetGroupType(
					current_group, &err);
			if (std::string { current_group_type } == "cell")
				parser::read_cell(current_group, library, time_unit, capacitive_load_unit);
		}
		si2drIterQuit(library_groups, &err);
	}
	si2drIterQuit(groups, &err);

}

} /* namespace liberty */
} /* namespace timing */
} /* namespace openeda */

