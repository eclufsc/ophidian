#include "LibertyParser.h"

#include <vector>
#include <fstream>
#include <3rdparty/liberty-parser/include/si2dr_liberty.h>
#include <cassert>
#include <iostream>

namespace ophidian
{
namespace parser
{

si2drErrorT err;

LibertyParser::LibertyParser()
{

}

LibertyParser::~LibertyParser()
{

}

void read_cell(si2drGroupIdT cell, Liberty *liberty){

//    si2drNamesIdT cell_names = si2drGroupGetNames(cell, &err);
//    std::string name { si2drIterNextName(cell_names, &err) };

//    si2drIterQuit(cell_names, &err);


//    auto cell_entity = library.cell_create(name);

//    si2drGroupsIdT groups = si2drGroupGetGroups(cell, &err);
//    si2drGroupIdT current_cell_group;

//    while (!si2drObjectIsNull((current_cell_group = si2drIterNextGroup(groups, &err)), &err)) {
//        std::string current_cell_group_type { si2drGroupGetGroupType(current_cell_group, &err) };
//        if(current_cell_group_type == "ff")
//            library.cell_sequential(cell_entity, true);
//        else if (current_cell_group_type == "pin") {
//            read_pin(cell_entity, current_cell_group, library, time_unit, capacitive_load_unit);
//        }
//    }

//    //    std::cout << "  flipflop: " << (cell_type.isSequential?"true":"false") << std::endl;

//    si2drIterQuit(groups, &err);

}

Liberty *LibertyParser::readFile(const std::string &filename)
{
    auto inp = std::make_unique<Liberty>();
    Liberty* liberty = inp.get();


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

        while (!si2drObjectIsNull((lib_attr = si2drIterNextAttr(lib_attrs, &err)), &err)) {
            std::string attr_name { si2drAttrGetName(lib_attr, &err) };
            if (attr_name == "time_unit") {
                liberty->time_unit = si2drSimpleAttrGetStringValue(lib_attr, &err);
            } else if (attr_name == "capacitive_load_unit") {
                si2drValuesIdT values = si2drComplexAttrGetValues(lib_attr, &err);
                si2drValueTypeT type;
                si2drInt32T int_val;
                si2drFloat64T double_val;
                si2drStringT string_val;
                si2drBooleanT bool_val;
                si2drExprT *expr;

                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
                si2drIterNextComplexValue(values, &type, &int_val, &double_val, &string_val, &bool_val, &expr, &err);
                std::string str_value { string_val };
                si2drIterQuit(values, &err);

                liberty->capacitive_load_unit = str_value;
                liberty->capacitive_load_unit_value = int_val;
            }
        }

        si2drIterQuit(lib_attrs, &err);
        while (!si2drObjectIsNull((current_group = si2drIterNextGroup(library_groups, &err)), &err)) {
            si2drStringT current_group_type = si2drGroupGetGroupType(current_group, &err);
            if (std::string { current_group_type } == "cell")
                read_cell(current_group, liberty);
        }
        si2drIterQuit(library_groups, &err);
    }
    si2drIterQuit(groups, &err);
    si2drPIQuit(&err);

    return liberty;
}

}
}
