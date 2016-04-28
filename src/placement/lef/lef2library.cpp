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

