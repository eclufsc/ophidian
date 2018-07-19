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

#ifndef OPHIDIAN_TIMING_WIREMODELS_H
#define OPHIDIAN_TIMING_WIREMODELS_H

#include <ophidian/util/Units.h>
#include <ophidian/timingdriven_placement/RCTree.h>
#include <ophidian/timing/Elmore.h>

namespace ophidian
{
namespace timing
{
namespace wiremodel
{

using rctree_type                      = timingdriven_placement::RCTree;
template <class T> using node_map_type = rctree_type::capacitor_map_type<T>;

using slew_unit_type                   = util::second_t;
using capacitance_unit_type            = rctree_type::capacitance_unit_type;
using precicion_type                   = double;
using slew_map_type                    = node_map_type<slew_unit_type>;
using capacitance_map_type             = node_map_type<capacitance_unit_type>;

class EffectiveCapacitance
{
public:

    EffectiveCapacitance();
    virtual ~EffectiveCapacitance();

    void precision(precicion_type epsilon);

    void slews(slew_map_type& sm);
    const slew_map_type& slews() const;

    void delays(slew_map_type& dm);
    const slew_map_type& delays() const;

    void ceff(capacitance_map_type &cm);
    const capacitance_map_type& ceff() const;

    template <class SlewCalculator>
    capacitance_unit_type simulate(const SlewCalculator & slew_calculator, rctree_type& tree)
    {
        if(!m_slews)
            m_slews = new slew_map_type(tree.g(), slew_unit_type(0));
        if(!m_delays)
            m_delays = new slew_map_type(tree.g(), slew_unit_type(0));
        if(!m_ceff)
            m_ceff = new capacitance_map_type(tree.g(), capacitance_unit_type(0));

        slew_map_type&        slews  = *m_slews;
        slew_map_type&        delays = *m_delays;
        capacitance_map_type& ceff   = *m_ceff;

        precicion_type error = 1.0;

        capacitance_unit_type current_ceff;
        delays[tree.source()] = slew_unit_type(0.0);

        const auto & order = tree.order();

        while (error > m_precision) {
            current_ceff = ceff[tree.source()];
            slews[tree.source()] = slew_calculator(current_ceff);

            for(auto current : order)
            {
                if (tree.g().id(current) == tree.g().id(tree.source()))
                    continue;

                auto parent = tree.pred(current);
                auto resistance_with_parent = tree.resistance(tree.resistor(parent, current));

                slews[current] = slews[parent];

                if(slews[parent] > slew_unit_type(0.0))
                {
                    precicion_type x = resistance_with_parent * ceff[current] / slews[parent];
                    slews[current] = slews[parent] / (1 - x * (1 - std::exp(-1 / x)));
                }

                delays[current] = delays[parent] + resistance_with_parent * ceff[current];
            }

            for(auto node : order)
                ceff[node] = tree.capacitance(node);

            for(auto current = order.rbegin(); current != order.rend(); ++current)
            {
                if (tree.g().id(*current) == tree.g().id(tree.source()))
                    continue;

                auto parent = tree.pred(*current);
                auto resistance_with_parent = tree.resistance(tree.resistor(parent, *current));
                precicion_type x = 2.0 * resistance_with_parent  * ceff[*current] / slews[parent];
                precicion_type y = 1.0 - std::exp(-1.0 / x);
                precicion_type shielding_factor = (slews[parent] > slew_unit_type(0.0)? 1.0 - x * y : 1.0);
                ceff[parent] += shielding_factor * ceff[*current];
            }

            error = std::abs(((current_ceff - ceff[tree.source()])
                             / std::max(current_ceff, ceff[tree.source()])).to<double>());
        }

        return current_ceff;
    }

private:
    bool                  m_slews_owner;
    bool                  m_delays_owner;
    bool                  m_ceff_owner;
    precicion_type        m_precision;

    slew_map_type*        m_slews;
    slew_map_type*        m_delays;
    capacitance_map_type* m_ceff;
};

class LumpedCapacitance
{
public:
    LumpedCapacitance();
    virtual ~LumpedCapacitance();

    void slews(slew_map_type& sm);
    const slew_map_type& slews() const;

    void delays(slew_map_type& dm);
    const slew_map_type& delays() const;

    void ceff(capacitance_map_type &cm);
    const capacitance_map_type& ceff() const;

    template <class SlewCalculator>
    capacitance_unit_type simulate(const SlewCalculator & slew_calculator, const rctree_type& tree)
    {
        if(!m_slews)
            m_slews = new slew_map_type(tree.g(), slew_unit_type(0));
        if(!m_delays)
            m_delays = new slew_map_type(tree.g(), slew_unit_type(0));
        if(!m_ceff)
            m_ceff = new capacitance_map_type(tree.g(), capacitance_unit_type(0));

        slew_map_type&        slews  = *m_slews;
        slew_map_type&        delays = *m_delays;
        capacitance_map_type& ceff   = *m_ceff;

        capacitance_unit_type lumped(0);
        for(rctree_type::graph_type::NodeIt it(tree.g()); it != lemon::INVALID; ++it)
            lumped += tree.capacitance(it);

        auto source_slew = slew_calculator(lumped);

        Elmore delay(tree);
        ElmoreSecondMoment second_moment(delay);

        for(auto it = delay.order().cbegin(); it != delay.order().cend(); ++it)
        {
            delays[*it] = delay.at(*it);
            auto step_slew = units::math::sqrt(second_moment.at(*it) * 2.0 - units::math::pow<2>(delay.at(*it)));
            slews[*it] = units::math::sqrt(units::math::pow<2>(source_slew) + units::math::pow<2>(step_slew));
        }

        return lumped;
    }

private:
    bool                  m_slews_owner;
    bool                  m_delays_owner;
    bool                  m_ceff_owner;
    slew_map_type*        m_slews;
    slew_map_type*        m_delays;
    capacitance_map_type* m_ceff;
};

}   // namespace WireModel
}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_WIREMODELS_H
