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

#include "lef2library.h"

namespace ophidian {
namespace placement {

void lef2library(const lef_parser &m_lef, library &lib)
{
    lib.dist2microns(m_lef.database_units());
    for(const lef_parser::macro & macro : m_lef.macros())
    {
        auto cell = lib.cell_create(macro.name);
        geometry::multi_polygon< geometry::polygon<geometry::point<double> > > cell_geometry;
        for(const lef_parser::pin & pin : macro.pins)
        {
            auto pin_entity = lib.pin_create(cell, pin.name);
            geometry::multi_polygon< geometry::polygon<geometry::point<double> > > pin_geometry;
            for(const lef_parser::port & port : pin.ports)
            {
                for(const lef_parser::rect & rect : port.rects)
                {
                    std::vector< geometry::point<double> >  points{
                        {rect.xl, rect.yl},
                        {rect.xl, rect.yh},
                        {rect.xh, rect.yh},
                        {rect.xh, rect.yl},
                        {rect.xl, rect.yl}
                    };
                    geometry::polygon<geometry::point<double> > pin_rectangle;
                    geometry::append(pin_rectangle, points);
                    pin_geometry.push_back(pin_rectangle);
                }
            }
            geometry::box<geometry::point<double> > box;
            geometry::envelope(pin_geometry, box);
            geometry::point<double> offset;
            offset.x( 0.5*(box.max_corner().x()+box.min_corner().x())*m_lef.database_units() ); // to dbu
            offset.y( 0.5*(box.max_corner().y()+box.min_corner().y())*m_lef.database_units() ); // to dbu
            lib.pin_offset(pin_entity, offset);
        }
        auto result = macro.obses.layer2rects.find("metal1");
        if(result != macro.obses.layer2rects.end())
        {
            for(auto & rect : result->second)
            {
                std::vector< geometry::point<double> >  points{
                    {rect.xl*m_lef.database_units(), rect.yl*m_lef.database_units()},
                    {rect.xl*m_lef.database_units(), rect.yh*m_lef.database_units()},
                    {rect.xh*m_lef.database_units(), rect.yh*m_lef.database_units()},
                    {rect.xh*m_lef.database_units(), rect.yl*m_lef.database_units()},
                    {rect.xl*m_lef.database_units(), rect.yl*m_lef.database_units()}
                };
                geometry::polygon<geometry::point<double> > polygon;
                geometry::append(polygon, points);
                cell_geometry.push_back(polygon);
            }
        }

        if(cell_geometry.empty())
        {
            std::vector< geometry::point<double> >  bounding{
                {macro.origin.x*m_lef.database_units(), macro.origin.y*m_lef.database_units()},
                {macro.origin.x*m_lef.database_units(), (macro.origin.y+macro.size.y)*m_lef.database_units()},
                {(macro.origin.x+macro.size.x)*m_lef.database_units(), (macro.origin.y+macro.size.y)*m_lef.database_units()},
                {(macro.origin.x+macro.size.x)*m_lef.database_units(), macro.origin.y*m_lef.database_units()},
                {macro.origin.x*m_lef.database_units(), macro.origin.y*m_lef.database_units()}
            };
            geometry::polygon<geometry::point<double> > polygon;
            geometry::append(polygon, bounding);
            cell_geometry.push_back(polygon);
        }
        lib.geometry(cell, cell_geometry);
    }
}


}
}

