#include "LibertyParser.h"
#include <3rdparty/libertyParser/include/si2dr_liberty.h>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <sstream>

namespace ophidian
{
namespace parser
{
class LibertyParser::Pimpl
{
public:
    std::vector<double> split_string_into_values(const std::string &the_string);
    void read_LUT(si2drGroupIdT group, Liberty::LUT & lut);
    void read_constraint(si2drGroupIdT group, Liberty::LUT &lut);
    void read_LUTs(si2drGroupIdT timing, Liberty::Timing &newTimming);
    Liberty::Timing readTiming(si2drGroupIdT timing, Liberty::Pin &pin);
    Liberty::Pin readPin(si2drGroupIdT pin, Liberty::Cell &cell);
    Liberty::Cell readCell(si2drGroupIdT cell);
public:
    Pimpl() {
        si2drPIInit(&err);
    }
    ~Pimpl(){
        si2drPIQuit(&err);
    }

public:
    si2drErrorT err;
};


LibertyParser::LibertyParser() :
     this_(std::make_unique<Pimpl>())
{

}

LibertyParser::~LibertyParser()
{

}
std::vector<double> LibertyParser::Pimpl::split_string_into_values(const std::string &the_string) {
    std::vector<double> values;
    std::stringstream input(the_string);
    std::string segment;
    while (std::getline(input, segment, ','))
    {
        values.push_back(std::stod(segment));
    }
    return values;
}

void LibertyParser::Pimpl::read_LUT(si2drGroupIdT group, Liberty::LUT & lut) {
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

void LibertyParser::Pimpl::read_constraint(si2drGroupIdT group, Liberty::LUT &lut)
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

void LibertyParser::Pimpl::read_LUTs(si2drGroupIdT timing, Liberty::Timing &newTimming) {
    si2drGroupsIdT groups = si2drGroupGetGroups(timing, &err);
    si2drGroupIdT group;
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
        std::string group_type { si2drGroupGetGroupType(group, &err) };
        Liberty::LUT lut;
        if (group_type == "rise_constraint") {
            {
                lut._lutInformation = Liberty::LUT::lutInformation::RISE_CONSTRAINT;
                read_constraint(group, lut);
            }
        } else if (group_type == "fall_constraint") {
            {
                lut._lutInformation = Liberty::LUT::lutInformation::FALL_CONSTRAINT;
                read_constraint(group, lut);
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
            read_LUT(group, lut);
        }
        newTimming.luts.push_back(lut);
    }
    si2drIterQuit(groups, &err);
}

Liberty::Timing LibertyParser::Pimpl::readTiming(si2drGroupIdT timing, Liberty::Pin &pin) {

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

    read_LUTs(timing, newTiming);
    return newTiming;
}

Liberty::Pin LibertyParser::Pimpl::readPin(si2drGroupIdT pin, Liberty::Cell &cell) {

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
            cell.sequential = true;
        }
    }
    si2drIterQuit(attrs, &err);

    if (std::string(direction) == "input")
        newPin.pinDirection = Liberty::Pin::directionPin::INPUT;
    else if (std::string(direction) == "output")
        newPin.pinDirection = Liberty::Pin::directionPin::OUTPUT;

    si2drGroupsIdT groups = si2drGroupGetGroups(pin, &err);
    si2drGroupIdT group;
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)), &err)) {
        std::string groupType { si2drGroupGetGroupType(group, &err) };
        if (groupType == "timing") {
            auto timing = readTiming(group, newPin);
            newPin.timing.push_back(timing);
        }
    }
    si2drIterQuit(groups, &err);
    return newPin;
}

Liberty::Cell LibertyParser::Pimpl::readCell(si2drGroupIdT cell){

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
            auto pin = readPin(currentCellGroup, newCell);
            newCell.pins.push_back(pin);
        }
    }
    si2drIterQuit(groups, &err);
    return newCell;
}

std::shared_ptr<Liberty> LibertyParser::readFile(const std::string &filename)
{
    std::shared_ptr<Liberty> lib = std::make_shared<Liberty>();
    Liberty * liberty = lib.get();

    si2drReadLibertyFile(const_cast<char*>(filename.c_str()), &this_->err);
    if (this_->err == SI2DR_INVALID_NAME) {
        std::cout << "COULD NOT OPEN " << filename << " for parsing-- quitting...\n";
        assert(false);
    } else if (this_->err == SI2DR_SYNTAX_ERROR) {
        std::cout << "\nSyntax Errors were detected in the input file!\n\n";
        assert(false);
    }
    si2drGroupsIdT groups = si2drPIGetGroups(&this_->err);
    si2drGroupIdT group;
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &this_->err)), &this_->err)) {
        si2drCheckLibertyLibrary(group, &this_->err);
        if (this_->err == SI2DR_NO_ERROR) {
        } else
            assert(false);
    }
    si2drIterQuit(groups, &this_->err);
    groups = si2drPIGetGroups(&this_->err);
    while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &this_->err)), &this_->err)) {
        si2drNamesIdT gnames;
        si2drStringT gname;
        si2drGroupsIdT libraryGroups;
        si2drGroupIdT current_group;
        gnames = si2drGroupGetNames(group, &this_->err);
        gname = si2drIterNextName(gnames, &this_->err);
        si2drIterQuit(gnames, &this_->err);
        std::cout << "Liberty Library Name: " << (gname ? gname : "<NONAME>") << std::endl;
        libraryGroups = si2drGroupGetGroups(group, &this_->err);
        si2drAttrsIdT lib_attrs = si2drGroupGetAttrs(group, &this_->err);
        si2drAttrIdT lib_attr;

        while (!si2drObjectIsNull((lib_attr = si2drIterNextAttr(lib_attrs, &this_->err)), &this_->err)) {
            std::string attr_name { si2drAttrGetName(lib_attr, &this_->err) };
            if (attr_name == "time_unit") {
                liberty->timeUnit = si2drSimpleAttrGetStringValue(lib_attr, &this_->err);
            } else if (attr_name == "capacitive_load_unit") {
                si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr, &this_->err);
                si2drValueTypeT type;
                si2drInt32T int_val;
                si2drFloat64T double_val;
                si2drStringT string_val;
                si2drBooleanT bool_val;
                si2drExprT *expr;

                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &this_->err);
                liberty->capacitiveLoadUnitValue = int_val;
                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &this_->err);
                std::string str_value { string_val };
                si2drIterQuit(values, &this_->err);

                liberty->capacitiveLoadUnit = str_value;
            }
        }

        si2drIterQuit(lib_attrs, &this_->err);
        while (!si2drObjectIsNull((current_group = si2drIterNextGroup(libraryGroups, &this_->err)), &this_->err)) {
            si2drStringT current_group_type = si2drGroupGetGroupType(current_group, &this_->err);
            if (std::string { current_group_type } == "cell"){
                auto cell = this_->readCell(current_group);
                liberty->cells.push_back(cell);
            }
        }
        si2drIterQuit(libraryGroups, &this_->err);
    }
    si2drIterQuit(groups, &this_->err);


    return lib;
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

double Liberty::LUT::compute(double rv, double cv)
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

}
}
