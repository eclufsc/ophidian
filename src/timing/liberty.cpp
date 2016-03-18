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

std::vector<double> split_string_into_values(const std::string &the_string) {
	std::vector<double> values;
	std::stringstream input(the_string);
	std::string segment;
	while (std::getline(input, segment, ','))
		values.push_back(std::stod(segment));
	return values;
}

void read_LUT(si2drGroupIdT group, boost::units::quantity<boost::units::si::time> time_unit, boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit, library::LUT*& lut) {

	si2drAttrsIdT attrs = si2drGroupGetAttrs(group, &err);
	si2drAttrIdT attr;

	std::vector<boost::units::quantity<boost::units::si::capacitance> > loadValues(8);
	std::vector<boost::units::quantity<boost::units::si::time> > slewValues(8);
	loadValues.resize(0);
	slewValues.resize(0);

	while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {

		std::string name { si2drAttrGetName(attr, &err) };
		if (name == "index_1" || name == "index_2" || name == "values") {
			si2drValuesIdT values = si2drComplexAttrGetValues(attr, &err);
			si2drValueTypeT type;
			si2drInt32T int_val;
			si2drFloat64T double_val;
			si2drStringT string_val;
			si2drBooleanT bool_val;
			si2drExprT *expr;
			si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);

			std::size_t i = 0;
			do {
				std::vector<double> values_vector = split_string_into_values(std::string { string_val });

				if (name == "index_1") {
					for (auto v : values_vector)
						loadValues.push_back(v * capacitive_load_unit);
				} else if (name == "index_2") {
					for (auto v : values_vector)
						slewValues.push_back(v * time_unit);
				} else if (name == "values") {

					if (!lut)
						lut = new library::LUT(std::max(static_cast<std::size_t>(1), loadValues.size()), std::max(static_cast<std::size_t>(1), slewValues.size()));
					assert(i < lut->row_count());
					assert(lut);
					for (std::size_t j = 0; j < values_vector.size(); ++j)
						lut->at(i, j, values_vector[j] * time_unit);
					++i;
				}
				si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
			} while (string_val);
			si2drIterQuit(values, &err);
		}
	}
	assert(lut);
	si2drIterQuit(attrs, &err);
}

void read_LUTs(si2drGroupIdT timing, entity::entity arc, library& library, boost::units::quantity<boost::units::si::time> time_unit, boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	si2drGroupsIdT groups = si2drGroupGetGroups(timing, &err);
	si2drGroupIdT group;

	while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
		std::string group_type { si2drGroupGetGroupType(group, &err) };
		if (group_type == "rise_constraint") {
			//	            __read_constraint(group, timing_info.riseConstraint);
		} else if (group_type == "fall_constraint") {
			//	            __read_constraint(group, timing_info.fallConstraint);
		} else {

			library::LUT * lut { nullptr };
			read_LUT(group, time_unit, capacitive_load_unit, lut);
			assert(lut);
			if (group_type == "cell_fall") {
				library.timing_arc_fall_delay(arc, *lut);
			} else if (group_type == "cell_rise") {
				library.timing_arc_rise_delay(arc, *lut);
			} else if (group_type == "fall_transition") {
				library.timing_arc_fall_slew(arc, *lut);
			} else if (group_type == "rise_transition") {
				library.timing_arc_rise_slew(arc, *lut);
			} else
				assert(false);
			delete lut;
		}
	}
	si2drIterQuit(groups, &err);

}

void read_timing(si2drGroupIdT timing, entity::entity pin_entity, library& library, boost::units::quantity<boost::units::si::time> time_unit, boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	std::string timing_sense { "negative_unate" };
	std::string timing_type { "combinational" };
	std::string related_pin { "default_related_pin" };
	si2drAttrsIdT attrs = si2drGroupGetAttrs(timing, &err);
	si2drAttrIdT attr;
	entity::entity from;

	while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {
		std::string attr_name { si2drAttrGetName(attr, &err) };
		if (attr_name == "timing_sense")
			timing_sense = si2drSimpleAttrGetStringValue(attr, &err);
		else if (attr_name == "timing_type")
			timing_type = si2drSimpleAttrGetStringValue(attr, &err);
		else if (attr_name == "related_pin") {
			related_pin = si2drSimpleAttrGetStringValue(attr, &err);
			from = library.std_cells().pin_create(library.std_cells().pin_owner(pin_entity), related_pin);
		}
	}
	si2drIterQuit(attrs, &err);
	auto arc = library.timing_arc_create(from, pin_entity);

	//	timing_info.timingSense = timing_sense;
	//	timing_info.timing_type = Liberty_Timing_Type::Combinational;
	//	if(timing_type == "setup_rising")
	//		timing_info.timing_type = Liberty_Timing_Type::Setup_Rising;
	//	else if(timing_type == "hold_rising")
	//		timing_info.timing_type = Liberty_Timing_Type::Hold_Rising;
	//	timing_info.fromPin = related_pin;
	//	timing_info.fallConstraint = std::numeric_limits<double>::max();
	//	timing_info.riseConstraint = std::numeric_limits<double>::max();

	//    std::cout << "    timing arc (" << timing_sense << ")" << std::endl;
	//    std::cout << "      related pin: " << related_pin << std::endl;
	//    std::cout << "      timing sense: " << timing_sense << std::endl;
	//    std::cout << "      timing type: " << timing_type << std::endl;

	read_LUTs(timing, arc, library, time_unit, capacitive_load_unit);

}

void read_pin(entity::entity cell_entity, si2drGroupIdT pin, library& library, boost::units::quantity<boost::units::si::time> time_unit, boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	si2drNamesIdT current_cell_group_names = si2drGroupGetNames(pin, &err);
	std::string pin_name { si2drIterNextName(current_cell_group_names, &err) };
	si2drIterQuit(current_cell_group_names, &err);

	auto pin_entity = library.pin_create(cell_entity, pin_name);

	si2drAttrsIdT attrs = si2drGroupGetAttrs(pin, &err);
	si2drAttrIdT attr;

	while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {
		std::string attr_name { si2drAttrGetName(attr, &err) };
		//		if (attr_name == "direction")
		//			direction = si2drSimpleAttrGetStringValue(attr, &err);
		//		else if (attr_name == "max_capacitance")
		//			max_capacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
		if (attr_name == "capacitance")
			library.pin_capacitance(pin_entity, si2drSimpleAttrGetFloat64Value(attr, &err) * capacitive_load_unit);
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
	si2drGroupsIdT groups = si2drGroupGetGroups(pin, &err);
	si2drGroupIdT group;
	while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
		std::string group_type { si2drGroupGetGroupType(group, &err) };
		if (group_type == "timing") {
			//			Liberty_Library_Timing_Info timing_info;
			//			timing_info.toPin = cell_pin.name;
			read_timing(group, pin_entity, library, time_unit, capacitive_load_unit);
			//			cell_type.timingArcs.push_back(timing_info);
		}
	}
	si2drIterQuit(groups, &err);
}

void read_cell(si2drGroupIdT cell, timing::library& library, boost::units::quantity<boost::units::si::time> time_unit, boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit) {

	si2drNamesIdT cell_names = si2drGroupGetNames(cell, &err);
	std::string name { si2drIterNextName(cell_names, &err) };

	si2drIterQuit(cell_names, &err);

	auto cell_entity = library.cell_create(name);

	si2drGroupsIdT groups = si2drGroupGetGroups(cell, &err);
	si2drGroupIdT current_cell_group;

	while (!si2drObjectIsNull((current_cell_group = si2drIterNextGroup(groups, &err)), &err)) {
		std::string current_cell_group_type { si2drGroupGetGroupType(current_cell_group, &err) };
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
		std::cout << "COULD NOT OPEN " << filename << " for parsing-- quitting...\n";
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
		std::cout << "Liberty Library Name: " << (gname ? gname : "<NONAME>") << std::endl;
		library_groups = si2drGroupGetGroups(group, &err);
		si2drAttrsIdT lib_attrs = si2drGroupGetAttrs(group, &err);
		si2drAttrIdT lib_attr;

		boost::units::quantity<boost::units::si::time> time_unit;
		boost::units::quantity<boost::units::si::capacitance> capacitive_load_unit;

		while (!si2drObjectIsNull((lib_attr = si2drIterNextAttr(lib_attrs, &err)), &err)) {
			std::string attr_name { si2drAttrGetName(lib_attr, &err) };
			if (attr_name == "time_unit") {
				std::string time_unit_string = si2drSimpleAttrGetStringValue(lib_attr, &err);
				if (time_unit_string == "1ps")
					time_unit = boost::units::quantity<boost::units::si::time>(1.0 * boost::units::si::pico * boost::units::si::second);
				else if (time_unit_string == "1ns")
					time_unit = boost::units::quantity<boost::units::si::time>(1.0 * boost::units::si::nano * boost::units::si::second);
				else if (time_unit_string == "1us")
					time_unit = boost::units::quantity<boost::units::si::time>(1.0 * boost::units::si::micro * boost::units::si::second);
				else if (time_unit_string == "1ms")
					time_unit = boost::units::quantity<boost::units::si::time>(1.0 * boost::units::si::milli * boost::units::si::second);
				else if (time_unit_string == "1s")
					time_unit = boost::units::quantity<boost::units::si::time>(1.0 * boost::units::si::second);
				else
					assert(false);
			} else if (attr_name == "capacitive_load_unit") {
				si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr, &err);
				si2drValueTypeT type;
				si2drInt32T int_val;
				si2drFloat64T double_val;
				si2drStringT string_val;
				si2drBooleanT bool_val;
				si2drExprT *expr;

				si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
				auto int_value = int_val;
				si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
				std::string str_value { string_val };
				si2drIterQuit(values, &err);

				if (str_value == "ff")
					capacitive_load_unit = boost::units::quantity<boost::units::si::capacitance>(int_value * boost::units::si::femto * boost::units::si::farads);
				else if (str_value == "pf")
					capacitive_load_unit = boost::units::quantity<boost::units::si::capacitance>(int_value * boost::units::si::pico * boost::units::si::farads);
				else if (str_value == "nf")
					capacitive_load_unit = boost::units::quantity<boost::units::si::capacitance>(int_value * boost::units::si::nano * boost::units::si::farads);
				else
					assert(false);
			}

		}

		si2drIterQuit(lib_attrs, &err);
		while (!si2drObjectIsNull((current_group = si2drIterNextGroup(library_groups, &err)), &err)) {
			si2drStringT current_group_type = si2drGroupGetGroupType(current_group, &err);
			if (std::string { current_group_type } == "cell")
				parser::read_cell(current_group, library, time_unit, capacitive_load_unit);
		}
		si2drIterQuit(library_groups, &err);
	}
	si2drIterQuit(groups, &err);
	si2drPIQuit(&err);
}

} /* namespace liberty */
} /* namespace timing */
} /* namespace openeda */

