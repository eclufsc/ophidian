#include "LibertyParser.h"
#include <3rdparty/liberty-parser/include/si2dr_liberty.h>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>

namespace ophidian
{
namespace parser
{



class LibertyParser::Pimpl
{
public:
    Liberty::pin read_pin(si2drGroupIdT pin);
    Liberty::cell read_cell(si2drGroupIdT cell);
public:
    Pimpl() {
        si2drPIInit(&err);
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



Liberty::pin LibertyParser::Pimpl::read_pin(si2drGroupIdT pin) {

    si2drNamesIdT current_cell_group_names = si2drGroupGetNames(pin, &err);
    std::string pin_name { si2drIterNextName(current_cell_group_names, &err) };
    si2drIterQuit(current_cell_group_names, &err);
    si2StringT direction;

    Liberty::pin newPin;
    newPin.name = pin_name;

    si2drAttrsIdT attrs = si2drGroupGetAttrs(pin, &err);
    si2drAttrIdT attr;

    while (!si2drObjectIsNull((attr = si2drIterNextAttr(attrs, &err)), &err)) {
        std::string attr_name { si2drAttrGetName(attr, &err) };
        if (attr_name == "direction"){
            direction = si2drSimpleAttrGetStringValue(attr, &err);
        }else if (attr_name == "max_capacitance"){
            newPin.max_capacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
        }else if (attr_name == "capacitance"){
            newPin.capacitance = si2drSimpleAttrGetFloat64Value(attr, &err);
        }else if (attr_name == "clock")
        {
            newPin.pinDirection = Liberty::pin::directionPin::INPUT;
            //TODO!
//            library.cell_sequential(cell_entity, true);
        }
    }
    si2drIterQuit(attrs, &err);

    if (std::string(direction) == "input")
        newPin.pinDirection = Liberty::pin::directionPin::INPUT;
    else if (std::string(direction) == "output")
        newPin.pinDirection = Liberty::pin::directionPin::OUTPUT;

    //    std::cout << "    direction: " << direction << std::endl;
    //    std::cout << "    max_capacitance: " << max_capacitance << std::endl;
    //    std::cout << "    capacitance: " << pin_capacitance << std::endl;

    //	cell_pin.isInput = (direction == "input");
    //  cell_pin.maxCapacitance = max_capacitance*capacitive_unit_;
    //  cell_pin.capacitance = pin_capacitance*capacitive_unit_;
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
            //TODO!
//            read_timing(group, pin_entity, library, time_unit, capacitive_load_unit);
        }
    }
    si2drIterQuit(groups, &err);
}

Liberty::cell LibertyParser::Pimpl::read_cell(si2drGroupIdT cell){

    si2drNamesIdT cell_names = si2drGroupGetNames(cell, &err);
    std::string name { si2drIterNextName(cell_names, &err) };

    si2drIterQuit(cell_names, &err);

    Liberty::cell newCell;
    newCell.name = name;

    si2drGroupsIdT groups = si2drGroupGetGroups(cell, &err);
    si2drGroupIdT current_cell_group;

    while (!si2drObjectIsNull((current_cell_group = si2drIterNextGroup(groups, &err)), &err)) {
        std::string current_cell_group_type { si2drGroupGetGroupType(current_cell_group, &err) };
        if(current_cell_group_type == "ff")
            newCell.sequential = true;
        else if (current_cell_group_type == "pin") {
            auto pin = read_pin(current_cell_group);
            newCell.pins.push_back(pin);
        }
    }
    si2drIterQuit(groups, &err);
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
        si2drGroupsIdT library_groups;
        si2drGroupIdT current_group;
        gnames = si2drGroupGetNames(group, &this_->err);
        gname = si2drIterNextName(gnames, &this_->err);
        si2drIterQuit(gnames, &this_->err);
        std::cout << "Liberty Library Name: " << (gname ? gname : "<NONAME>") << std::endl;
        library_groups = si2drGroupGetGroups(group, &this_->err);
        si2drAttrsIdT lib_attrs = si2drGroupGetAttrs(group, &this_->err);
        si2drAttrIdT lib_attr;

        while (!si2drObjectIsNull((lib_attr = si2drIterNextAttr(lib_attrs, &this_->err)), &this_->err)) {
            std::string attr_name { si2drAttrGetName(lib_attr, &this_->err) };
            if (attr_name == "time_unit") {
                liberty->time_unit = si2drSimpleAttrGetStringValue(lib_attr, &this_->err);
            } else if (attr_name == "capacitive_load_unit") {
                si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr, &this_->err);
                si2drValueTypeT type;
                si2drInt32T int_val;
                si2drFloat64T double_val;
                si2drStringT string_val;
                si2drBooleanT bool_val;
                si2drExprT *expr;

                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &this_->err);
                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &this_->err);
                std::string str_value { string_val };
                si2drIterQuit(values, &this_->err);

                liberty->capacitive_load_unit = str_value;
                liberty->capacitive_load_unit_value = int_val;
            }
        }

        si2drIterQuit(lib_attrs, &this_->err);
        while (!si2drObjectIsNull((current_group = si2drIterNextGroup(library_groups, &this_->err)), &this_->err)) {
            si2drStringT current_group_type = si2drGroupGetGroupType(current_group, &this_->err);
            if (std::string { current_group_type } == "cell"){
                auto cell = this_->read_cell(current_group);
                liberty->cells.push_back(cell);
            }
        }
        si2drIterQuit(library_groups, &this_->err);
    }
    si2drIterQuit(groups, &this_->err);
    si2drPIQuit(&this_->err);

    return lib;
}

}
}
