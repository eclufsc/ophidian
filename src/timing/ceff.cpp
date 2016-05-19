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

void effective_capacitance_wire_model::slew_map(std::vector<effective_capacitance_wire_model::SlewType> &sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

void effective_capacitance_wire_model::delay_map(std::vector<effective_capacitance_wire_model::SlewType> &dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void effective_capacitance_wire_model::ceff_map(std::vector<CapacitanceType> &cm)
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

void lumped_capacitance_wire_model::slew_map(std::vector<lumped_capacitance_wire_model::SlewType> &sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

void lumped_capacitance_wire_model::delay_map(std::vector<lumped_capacitance_wire_model::SlewType> &dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void lumped_capacitance_wire_model::ceff_map(std::vector<lumped_capacitance_wire_model::CapacitanceType> &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

}
}
