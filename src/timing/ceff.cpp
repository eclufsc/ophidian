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

#include "ceff.h"

namespace ophidian {
namespace timing {

ceff::ceff():
    m_precision(1e-6),
    m_slews(nullptr),
    m_delays(nullptr),
    m_ceff(nullptr),
    m_slews_owner(true),
    m_delays_owner(true),
    m_ceff_owner(true){

}

ceff::~ceff()
{
    if(m_ceff_owner && m_ceff) delete m_ceff;
    if(m_delays_owner && m_delays) delete m_delays;
    if(m_slews_owner && m_slews) delete m_slews;
}

void ceff::precision(double epsilon)
{
    m_precision = epsilon;
}

void ceff::slew_map(lemon::ListGraph::NodeMap<ceff::SlewType> &sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

void ceff::delay_map(lemon::ListGraph::NodeMap<ceff::SlewType> &dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void ceff::ceff_map(lemon::ListGraph::NodeMap<ceff::CapacitanceType> &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

}
}
