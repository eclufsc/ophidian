/*
 * Copyright 2016 Ophidian
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

#include "Liberty.h"

#include <si2dr_liberty.h>

// std headers
#include <cassert>
#include <iostream>
#include <sstream>

//helper functions for liberty parser
namespace ophidian::parser{
std::vector<double> split_string_into_values(const std::string &the_string) {
    std::vector<double> values;
    std::stringstream input(the_string);
    std::string segment;
    while (std::getline(input, segment, ','))
    {
        values.push_back(std::stod(segment));
    }
    return values;
}

void read_LUT(si2drGroupIdT group, Liberty::LUT & lut, si2drErrorT & err) {
    si2drAttrsIdT attrs = si2drGroupGetAttrs(group, &err);
    si2drAttrIdT attr;
    lut.index_1.reserve(8);
    lut.index_2.reserve(8);
    lut.values.reserve(8);
    while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err))
    {
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
            do {
                std::vector<double> values_vector = split_string_into_values(std::string { string_val });

                if (name == "index_1") {
                    for (auto v : values_vector)
                        lut.index_1.push_back(v);
                } else if (name == "index_2") {
                    for (auto v : values_vector)
                        lut.index_2.push_back(v);
                } else if (name == "values") {
                    lut.values.push_back(values_vector);
                }
                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
            } while (string_val);
            si2drIterQuit(values, &err);
        }
    }

    lut.index_1.shrink_to_fit();
    lut.index_2.shrink_to_fit();
    lut.values.shrink_to_fit();
    si2drIterQuit(attrs, &err);
}

void read_constraint(si2drGroupIdT group, Liberty::LUT &lut, si2drErrorT & err)
{
    si2drNamesIdT group_names{si2drGroupGetNames(group, &err)};
    std::string group_name{si2drIterNextName(group_names, &err)};
    si2drIterQuit(group_names, &err);
    assert(group_name == "scalar");
    si2drAttrsIdT attrs = si2drGroupGetAttrs(group, &err);
    si2drAttrIdT attr;
    double constraint;
    while( !si2drObjectIsNull((attr=si2drIterNextAttr(attrs,&err)),&err) )
    {
        std::string name{si2drAttrGetName(attr, &err)};
        assert(name=="values");
        si2drValuesIdT values = si2drComplexAttrGetValues(attr, &err);
        si2drValueTypeT type;
        si2drInt32T     int_val;
        si2drFloat64T   double_val;
        si2drStringT    string_val;
        si2drBooleanT   bool_val;
        si2drExprT      *expr;
        si2drIterNextComplexValue(values,
                                  &type,
                                  &int_val,
                                  &double_val,
                                  &string_val,
                                  &bool_val,
                                  &expr,
                                  &err);
        constraint = std::stod(string_val);
        lut.values.push_back({constraint});
        si2drIterQuit(values, &err);
    }
    si2drIterQuit(attrs, &err);
}

void read_LUTs(si2drGroupIdT timing, Liberty::Timing &newTimming, si2drErrorT & err) {
    si2drGroupsIdT groups = si2drGroupGetGroups(timing, &err);
    si2drGroupIdT group;
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
        std::string group_type { si2drGroupGetGroupType(group, &err) };
        Liberty::LUT lut;
        if (group_type == "rise_constraint") {
            {
                lut._lutInformation = Liberty::LUT::lutInformation::RISE_CONSTRAINT;
                read_constraint(group, lut, err);
            }
        } else if (group_type == "fall_constraint") {
            {
                lut._lutInformation = Liberty::LUT::lutInformation::FALL_CONSTRAINT;
                read_constraint(group, lut, err);
            }
        } else {
            if (group_type == "cell_fall") {
               lut._lutInformation = Liberty::LUT::lutInformation::CELL_FALL;
            } else if (group_type == "cell_rise") {
                lut._lutInformation = Liberty::LUT::lutInformation::CELL_RISE;
            } else if (group_type == "fall_transition") {
                lut._lutInformation = Liberty::LUT::lutInformation::FALL_TRANSITION;
            } else if (group_type == "rise_transition") {
                lut._lutInformation = Liberty::LUT::lutInformation::RISE_TRANSITION;
            } else
                assert(false);
            read_LUT(group, lut, err);
        }
        newTimming.luts.push_back(lut);
    }
    si2drIterQuit(groups, &err);
}

Liberty::Timing readTiming(si2drGroupIdT timing, Liberty::Pin &pin, si2drErrorT & err) {
    Liberty::Timing newTiming;
    std::string timing_sense { "negative_unate" };
    std::string timing_type { "combinational" };
    std::string related_pin { "default_related_pin" };
    si2drAttrsIdT attrs = si2drGroupGetAttrs(timing, &err);
    si2drAttrIdT attr;
    while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {
        std::string attr_name { si2drAttrGetName(attr, &err) };
        if (attr_name == "timing_sense") {
            timing_sense = si2drSimpleAttrGetStringValue(attr, &err);
        } else if (attr_name == "timing_type")
            timing_type = si2drSimpleAttrGetStringValue(attr, &err);
        else if (attr_name == "related_pin") {
            related_pin = si2drSimpleAttrGetStringValue(attr, &err);
            newTiming.relatedPin = related_pin;
        }
    }
    si2drIterQuit(attrs, &err);
    Liberty::Timing::type type = Liberty::Timing::type::COMBINATIONAL;
    if(timing_type=="setup_rising")
    {
        type = Liberty::Timing::type::SETUP_RISING;
    }
    else if(timing_type=="hold_rising")
    {
        type = Liberty::Timing::type::HOLD_RISING;
    }
    else if(timing_type=="rising_edge")
        type = Liberty::Timing::type::RISING_EDGE;
    newTiming.timingType = type;
    if (timing_sense == "negative_unate")
        newTiming.timingSense = Liberty::Timing::unateness::NEGATIVE_UNATE;
    else if (timing_sense == "positive_unate")
        newTiming.timingSense = Liberty::Timing::unateness::POSITIVE_UNATE;
    else if (timing_sense == "non_unate")
        newTiming.timingSense = Liberty::Timing::unateness::NON_UNATE;

    read_LUTs(timing, newTiming, err);
    return newTiming;
}

Liberty::Pin readPin(si2drGroupIdT pin, Liberty::Cell &cell, si2drErrorT & err) {
    si2drNamesIdT currentCellGroupNames = si2drGroupGetNames(pin, &err);
    std::string pinName { si2drIterNextName(currentCellGroupNames, &err) };
    si2drIterQuit(currentCellGroupNames, &err);
    si2StringT direction;
    Liberty::Pin newPin;
    newPin.name = pinName;
    si2drAttrsIdT attrs = si2drGroupGetAttrs(pin, &err);
    si2drAttrIdT attr;
    while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err))
    {
        std::string attrName { si2drAttrGetName(attr, &err) };
        if (attrName == "direction"){
            direction = si2drSimpleAttrGetStringValue(attr, &err);
        }else if (attrName == "max_capacitance"){
            newPin.maxCapacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
        }else if (attrName == "capacitance"){
            newPin.capacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
        }else if (attrName == "clock")
        {
            newPin.pinDirection = Liberty::Pin::directionPin::INPUT;
            newPin.clock = true;
            cell.sequential = true;
        }
    }
    si2drIterQuit(attrs, &err);
    if (std::string(direction) == "input")
        newPin.pinDirection = Liberty::Pin::directionPin::INPUT;
    else if (std::string(direction) == "output")
        newPin.pinDirection = Liberty::Pin::directionPin::OUTPUT;
    else if (std::string(direction) == "inout")
        newPin.pinDirection = Liberty::Pin::directionPin::INOUT;
    else if (std::string(direction) == "internal")
        newPin.pinDirection = Liberty::Pin::directionPin::INTERNAL;
    si2drGroupsIdT groups = si2drGroupGetGroups(pin, &err);
    si2drGroupIdT group;
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
        std::string groupType { si2drGroupGetGroupType(group, &err) };
        if (groupType == "timing") {
            auto timing = readTiming(group, newPin, err);
            newPin.timing.push_back(timing);
        }
    }
    si2drIterQuit(groups, &err);
    return newPin;
}

Liberty::Cell readCell(si2drGroupIdT cell, si2drErrorT err){
    si2drNamesIdT cellNames = si2drGroupGetNames(cell, &err);
    std::string name { si2drIterNextName(cellNames, &err) };
    si2drIterQuit(cellNames, &err);
    Liberty::Cell newCell;
    newCell.name = name;
    si2drGroupsIdT groups = si2drGroupGetGroups(cell, &err);
    si2drGroupIdT currentCellGroup;
    while (!si2drObjectIsNull((currentCellGroup = si2drIterNextGroup(groups, &err)), &err)) {
        std::string currentCellGroupType { si2drGroupGetGroupType(currentCellGroup, &err) };
        if(currentCellGroupType == "ff")
            newCell.sequential = true;
        else if (currentCellGroupType == "pin") {
            auto pin = readPin(currentCellGroup, newCell, err);
            newCell.pins.push_back(pin);
        }
    }
    si2drIterQuit(groups, &err);
    return newCell;
}

Liberty::Cell Liberty::find(std::string cellName)
{
    for (auto cell : cells)
        if (cell.name == cellName)
            return cell;

    return Liberty::Cell();
}

Liberty::Pin Liberty::Cell::find(std::string pinName)
{
    for (auto pin : pins)
        if (pin.name == pinName)
            return pin;

    return Liberty::Pin();
}

std::vector<Liberty::Pin> Liberty::Cell::inputs()
{
    std::vector<Pin> ins;
    for (auto pin : pins)
        if (pin.pinDirection == Liberty::Pin::INPUT)
            ins.push_back(pin);

    return ins;
}

std::vector<Liberty::Pin> Liberty::Cell::outputs()
{
    std::vector<Pin> outs;
    for (auto pin : pins)
        if (pin.pinDirection == Liberty::Pin::OUTPUT)
            outs.push_back(pin);

    return outs;
}

Liberty::Timing Liberty::Pin::find(Liberty::Timing::type timingT)
{
    for (auto tim : timing)
        if (tim.timingType == timingT)
            return tim;

    return Liberty::Timing();
}

Liberty::LUT Liberty::Timing::find(Liberty::LUT::lutInformation info)
{
    for (auto lut : luts)
        if (lut._lutInformation == info)
            return lut;

    return Liberty::LUT();
}

double Liberty::LUT::compute(const double rv, const double cv)
{
    if (values.size() == 1)
        if (values.front().size() == 1)
            return values.front().front();

    double wTransition, wLoad;
    double y1, y2;
    double x1, x2;
    double t[2][2];
    std::size_t row1, row2, column1, column2;

    wTransition = 0.0f;
    wLoad = 0.0f;

    row1 = index_1.size() - 2;
    row2 = index_1.size() - 1;
    y1 = index_1[row1];
    y2 = index_1[row2];

    // loads -- rows
    for (size_t i = 0; i < index_1.size() - 1; i++) {
        if (rv >= index_1[i] && rv <= index_1[i + 1]) {
            row1 = i;
            row2 = i + 1;
            y1 = index_1[row1];
            y2 = index_1[row2];
        }
    }

    // transitions -- columns
    if (cv < index_2[0]) {
        column1 = 0;
        column2 = 1;
        x1 = index_2[column1];
        x2 = index_2[column2];
    } else if (cv > index_2[index_2.size() - 1]) {
        column1 = index_2.size() - 2;
        column2 = index_2.size() - 1;
        x1 = index_2[column1];
        x2 = index_2[column2];
    } else {
        for (size_t i = 0; i < index_2.size() - 1; i++) {
            if (cv >= index_2[i] && cv <= index_2[i + 1]) {
                column1 = i;
                column2 = i + 1;
                x1 = index_2[column1];
                x2 = index_2[column2];
            }
        }
    }

    //equation for interpolation (Ref - ISPD Contest: http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf), slide 17
    wTransition = (cv - x1) / (x2 - x1);
    wLoad = (rv - y1) / (y2 - y1);

    t[0][0] = values[row1][column1];
    t[0][1] = values[row1][column2];
    t[1][0] = values[row2][column1];
    t[1][1] = values[row2][column2];

    return ((1 - wTransition) * (1 - wLoad) * t[0][0])
            + (wTransition * (1 - wLoad) * t[0][1])
            + ((1 - wTransition) * wLoad * t[1][0])
            + (wTransition * wLoad * t[1][1]);

}

Liberty::Liberty(const std::string& liberty_file){
    si2drErrorT err;
    si2drPIInit(&err);

    si2drReadLibertyFile(const_cast<char*>(liberty_file.c_str()), &err);
    if (err == SI2DR_INVALID_NAME) {
        std::cout << "COULD NOT OPEN " << liberty_file << " for parsing-- quitting...\n";
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
        si2drGroupsIdT libraryGroups;
        si2drGroupIdT current_group;
        gnames = si2drGroupGetNames(group, &err);
        gname = si2drIterNextName(gnames, &err);
        si2drIterQuit(gnames, &err);
//        std::cout << "Liberty Library Name: " << (gname ? gname : "<NONAME>") << std::endl;
        libraryGroups = si2drGroupGetGroups(group, &err);
        si2drAttrsIdT lib_attrs = si2drGroupGetAttrs(group, &err);
        si2drAttrIdT lib_attr;
        while (!si2drObjectIsNull((lib_attr = si2drIterNextAttr(lib_attrs, &err)), &err)) {
            std::string attr_name { si2drAttrGetName(lib_attr, &err) };
            if (attr_name == "time_unit") {
                timeUnit = si2drSimpleAttrGetStringValue(lib_attr, &err);
            } else if (attr_name == "capacitive_load_unit") {
                si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr, &err);
                si2drValueTypeT type;
                si2drInt32T int_val;
                si2drFloat64T double_val;
                si2drStringT string_val;
                si2drBooleanT bool_val;
                si2drExprT *expr;

                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
                capacitiveLoadUnitValue = int_val;
                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
                std::string str_value { string_val };
                si2drIterQuit(values, &err);

                capacitiveLoadUnit = str_value;
            }
        }
        si2drIterQuit(lib_attrs, &err);
        while (!si2drObjectIsNull((current_group = si2drIterNextGroup(libraryGroups, &err)), &err)) {
            si2drStringT current_group_type = si2drGroupGetGroupType(current_group, &err);
            if (std::string { current_group_type } == "cell"){
                auto cell = readCell(current_group, err);
                cells.push_back(cell);
            }
        }
        si2drIterQuit(libraryGroups, &err);
    }
    si2drIterQuit(groups, &err);

    si2drPIQuit(&err);
}


}
