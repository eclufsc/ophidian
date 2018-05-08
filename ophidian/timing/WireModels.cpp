#include "WireModels.h"

namespace ophidian
{
namespace timing
{
namespace wiremodel
{

EffectiveCapacitance::EffectiveCapacitance() :
    m_precision(1e-6),
    m_slews(nullptr),
    m_delays(nullptr),
    m_ceff(nullptr),
    m_slews_owner(true),
    m_delays_owner(true),
    m_ceff_owner(true)
{

}

EffectiveCapacitance::~EffectiveCapacitance()
{
    if(m_ceff_owner && m_ceff)
        delete m_ceff;

    if(m_delays_owner && m_delays)
        delete m_delays;

    if(m_slews_owner && m_slews)
        delete m_slews;
}

void EffectiveCapacitance::precision(precicion_type epsilon)
{
    m_precision = epsilon;
}

const slew_map_type& EffectiveCapacitance::slews() const
{
    return *m_slews;
}

void EffectiveCapacitance::slews(slew_map_type& sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

const slew_map_type& EffectiveCapacitance::delays() const
{
    return *m_delays;
}

void EffectiveCapacitance::delays(slew_map_type& dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void EffectiveCapacitance::ceff(capacitance_map_type &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

const capacitance_map_type& EffectiveCapacitance::ceff() const
{
    return *m_ceff;
}

LumpedCapacitance::LumpedCapacitance() :
    m_slews(nullptr),
    m_delays(nullptr),
    m_ceff(nullptr),
    m_slews_owner(true),
    m_delays_owner(true),
    m_ceff_owner(true)
{

}

LumpedCapacitance::~LumpedCapacitance()
{
    if(m_ceff_owner && m_ceff)
        delete m_ceff;

    if(m_delays_owner && m_delays)
        delete m_delays;

    if(m_slews_owner && m_slews)
        delete m_slews;
}

const slew_map_type& LumpedCapacitance::slews() const
{
    return *m_slews;
}

void LumpedCapacitance::slews(slew_map_type& sm)
{
    m_slews_owner = false;
    m_slews = &sm;
}

const slew_map_type& LumpedCapacitance::delays() const
{
    return *m_delays;
}

void LumpedCapacitance::delays(slew_map_type& dm)
{
    m_delays_owner = false;
    m_delays = &dm;
}

void LumpedCapacitance::ceff(capacitance_map_type &cm)
{
    m_ceff_owner = false;
    m_ceff = &cm;
}

const capacitance_map_type& LumpedCapacitance::ceff() const
{
    return *m_ceff;
}

}   // namespace wiremodel
}   // namespace timing
}   // namespace ophidian
