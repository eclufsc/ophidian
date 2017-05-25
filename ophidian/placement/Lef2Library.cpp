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

#include "Lef2Library.h"

namespace ophidian{
namespace placement{

void lef2Library(const parser::Lef & lef, Library & library, standard_cell::StandardCells & stdCells){
    for(auto & macro : lef.macros()){
        auto stdCell = stdCells.add(standard_cell::Cell(), macro.name);
        auto layer2RectsM1 = macro.obses.layer2rects.find("metal1");
        if(layer2RectsM1 != macro.obses.layer2rects.end()){
            geometry::MultiBox geometry;
            for(auto & rect : layer2RectsM1->second){
                ophidian::geometry::Point pmin = {rect.xl*lef.databaseUnits(), rect.yl*lef.databaseUnits()};
                ophidian::geometry::Point pmax = {rect.xh*lef.databaseUnits(), rect.yh*lef.databaseUnits()};
                geometry.push_back(ophidian::geometry::Box(pmin, pmax));
            }
            library.geometry(stdCell, geometry);
        }else{
            ophidian::geometry::Point pmin = {macro.origin.x*lef.databaseUnits(), macro.origin.y*lef.databaseUnits()};
            ophidian::geometry::Point pmax = {macro.size.x*lef.databaseUnits(), macro.size.y*lef.databaseUnits()};
            library.geometry(stdCell, geometry::MultiBox({ophidian::geometry::Box(pmin, pmax)}));
        }

        for(auto pin : macro.pins){
            auto stdPin = stdCells.add(standard_cell::Pin(), macro.name+":"+pin.name, standard_cell::PinDirection(pin.direction));
            stdCells.add(stdCell, stdPin);
            for(auto port : pin.ports)
                for(auto rect : port.rects)
                    library.pinOffset(stdPin, util::LocationMicron(0.5*(rect.xl+rect.xh)*lef.databaseUnits(), 0.5*(rect.yl+rect.yh)*lef.databaseUnits()));
        }
    }
}
}
}


