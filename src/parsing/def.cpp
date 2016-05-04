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

#include "def.h"
#include <iostream>

namespace ophidian {
namespace parsing {

def::def(const std::string &filename)
{
    defrInit();

    defrSetUnitsCbk([](defrCallbackType_e, double number, defiUserData ud)->int{
        def& that = *static_cast<def*>(ud);
        that.m_units = number;
        return 0;
    });

    defrSetDieAreaCbk([](defrCallbackType_e, defiBox *box, defiUserData ud)->int{
        def& that = *static_cast<def*>(ud);
        that.m_die.lower = {box->xl(), box->yl()};
        that.m_die.upper = {box->xh(), box->yh()};
        return 0;
    });

    defrSetRowCbk([](defrCallbackType_e, defiRow *defrow, defiUserData ud)->int{
        def& that = *static_cast<def*>(ud);
        row r;
        r.name = defrow->name();
        r.site = defrow->macro();
        r.num = {defrow->xNum(), defrow->yNum()};
        r.step = {defrow->xStep(), defrow->yStep()};
        r.origin = {defrow->x(), defrow->y()};
        that.m_rows.push_back(r);
        return 0;
    });

    defrSetComponentStartCbk([](defrCallbackType_e, int number, defiUserData ud)->int{
        def& that = *static_cast<def*>(ud);
        that.m_components.reserve(number);
        return 0;
    });

    defrSetComponentCbk([](defrCallbackType_e, defiComponent *comp, defiUserData ud)->int{
        def& that = *static_cast<def*>(ud);

        component c;
        c.name = comp->id();
        c.macro = comp->name();
        c.fixed = comp->isFixed();
        c.position = {comp->placementX(), comp->placementY()};
        c.orientation = comp->placementOrientStr();
        that.m_components.push_back(c);
        return 0;
    });


    FILE* ifp = fopen(filename.c_str(), "r");
    auto res = defrRead(ifp, filename.c_str(), this, true);
    if(res)
        std::cerr << "error" << std::endl;

    fclose(ifp);
}

def::~def()
{

}

} /* namespace parsing */
} /* namespace ophidian */

