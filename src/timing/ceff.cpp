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

effective_capacitance_wire_model::effective_capacitance_wire_model():
    m_precision(1e-6),
    m_slews(nullptr),
    m_delays(nullptr),
    m_ceff(nullptr),
    m_slews_owner(true),
    m_delays_owner(true),
    m_ceff_owner(true){

}

effective_capacitance_wire_model::~effective_capacitance_wire_model()
{
    if(m_ceff_owner && m_ceff) delete m_ceff;
    if(m_delays_owner && m_delays) delete m_delays;
    if(m_slews_owner && m_slews) delete m_slews;
}

void effective_capacitance_wire_model::precision(double epsilon)
{
    m_precision = epsilon;
}

void effective_capacitance_wire_model::slew_map(lemon::ListGraph::NodeMap<effective_capacitance_wire_model::SlewType> &sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

void effective_capacitance_wire_model::delay_map(lemon::ListGraph::NodeMap<effective_capacitance_wire_model::SlewType> &dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void effective_capacitance_wire_model::ceff_map(lemon::ListGraph::NodeMap<effective_capacitance_wire_model::CapacitanceType> &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

lumped_capacitance_wire_model::lumped_capacitance_wire_model():
    m_slews(nullptr),
    m_delays(nullptr),
    m_ceff(nullptr),
    m_slews_owner(true),
    m_delays_owner(true),
    m_ceff_owner(true)
{

}

lumped_capacitance_wire_model::~lumped_capacitance_wire_model()
{
    if(m_ceff_owner && m_ceff) delete m_ceff;
    if(m_delays_owner && m_delays) delete m_delays;
    if(m_slews_owner && m_slews) delete m_slews;
}

void lumped_capacitance_wire_model::slew_map(lemon::ListGraph::NodeMap<lumped_capacitance_wire_model::SlewType> &sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

void lumped_capacitance_wire_model::delay_map(lemon::ListGraph::NodeMap<lumped_capacitance_wire_model::SlewType> &dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void lumped_capacitance_wire_model::ceff_map(lemon::ListGraph::NodeMap<lumped_capacitance_wire_model::CapacitanceType> &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

}
}
