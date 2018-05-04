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

template <class T> using node_mapper_type = timingdriven_placement::RCTree::graph_type::NodeMap<T>;

using slew_type                           = util::second_t;
using capacitance_type                    = util::farad_t;
using precicion_type                      = double;
using slew_mapper_type                    = node_mapper_type<slew_type>;
using capacitance_mapper_type             = node_mapper_type<capacitance_type>;

class EffectiveCapacitance
{
public:

    EffectiveCapacitance();
    virtual ~EffectiveCapacitance();

    void precision(precicion_type epsilon);

    void slews(node_mapper_type<slew_type>& sm);
    const node_mapper_type<slew_type>& slews() const;

    void delays(node_mapper_type<slew_type>& dm);
    const node_mapper_type<slew_type>& delays() const;

    void ceff(node_mapper_type<capacitance_type> &cm);
    const node_mapper_type<capacitance_type>& ceff() const;

    template <class SlewCalculator>
    capacitance_type simulate(const SlewCalculator& slew_calculator, timingdriven_placement::RCTree& tree)
    {
        if(!m_slews)
            m_slews = new node_mapper_type<slew_type>(tree.g());
        if(!m_delays)
            m_delays = new node_mapper_type<slew_type>(tree.g());
        if(!m_ceff)
            m_ceff = new node_mapper_type<capacitance_type>(tree.g());

        node_mapper_type<slew_type>&        slews  = *m_slews;
        node_mapper_type<slew_type>&        delays = *m_delays;
        node_mapper_type<capacitance_type>& ceff   = *m_ceff;

        precicion_type error = 1.0;

        capacitance_type current_ceff;
        delays[tree.source()] = slew_type(0.0);

        while (error > m_precision) {
            current_ceff = ceff[tree.source()];
            slews[tree.source()] = slew_calculator(current_ceff);

            //////  USAR ITERADOR SOBRE OS NOS DO GRAFO DA ARVORE
            for(auto current : tree.order())
            {
                auto parent = tree.pred(current);
                auto resistance_with_parent = tree.resistance(tree.resistor(parent, current));

                slews[current] = slews[parent];
                if(slews[parent] > slew_type(0.0)){
                    precicion_type x = resistance_with_parent * ceff[current] / slews[parent];
                    slews[current] = slews[parent] / (1 - x * (1 - std::exp(-1 / x)));
                }

                delays[current] = delays[parent] + resistance_with_parent * ceff[current];
            }

            const auto & order = tree.order();

            for(auto node : order)
                ceff[node] = tree.capacitance(node);

            for(auto current = order.rbegin(); current != order.rend(); ++current)
            {
                auto parent = tree.pred(*current);
                auto resistance_with_parent = tree.resistance(tree.resistor(parent, *current));
                precicion_type x = 2.0 * resistance_with_parent  * ceff[*current] / slews[parent];
                precicion_type y = 1.0 - std::exp(-1.0 / x);
                precicion_type shielding_factor = (slews[parent] > slew_type(0.0)? 1.0 - x * y : 1.0);
                ceff[parent] += shielding_factor * ceff[*current];
            }

            error = std::abs(((current_ceff - ceff[tree.source()])
                             / std::max(current_ceff, ceff[tree.source()])).to<double>());
        }

        return current_ceff;
    }

private:
    bool                     m_slews_owner;
    bool                     m_delays_owner;
    bool                     m_ceff_owner;
    precicion_type           m_precision;

    slew_mapper_type*        m_slews;
    slew_mapper_type*        m_delays;
    capacitance_mapper_type* m_ceff;
};

class LumpedCapacitance
{
public:
    LumpedCapacitance();
    virtual ~LumpedCapacitance();

    void slews(node_mapper_type<slew_type>& sm);
    const node_mapper_type<slew_type>& slews() const;

    void delays(node_mapper_type<slew_type>& dm);
    const node_mapper_type<slew_type>& delays() const;

    void ceff(node_mapper_type<capacitance_type> &cm);
    const node_mapper_type<capacitance_type>& ceff() const;

    template <class SlewCalculator>
    capacitance_type simulate(const SlewCalculator & slew_calculator, timingdriven_placement::RCTree& tree)
    {
        if(!m_slews)
            m_slews = new node_mapper_type<slew_type>(tree.g());
        if(!m_delays)
            m_delays = new node_mapper_type<slew_type>(tree.g());
        if(!m_ceff)
            m_ceff = new node_mapper_type<capacitance_type>(tree.g());

        node_mapper_type<slew_type>&        slews  = *m_slews;
        node_mapper_type<slew_type>&        delays = *m_delays;
        node_mapper_type<capacitance_type>& ceff   = *m_ceff;


        capacitance_type lumped;
        for(timingdriven_placement::RCTree::graph_type::NodeIt it(tree.g()); it != lemon::INVALID; ++it)
            lumped += tree.capacitance(it);

        auto source_slew = slew_calculator(lumped);

        Elmore delay(tree);
        ElmoreSecondMoment second_moment(delay);

        for(timingdriven_placement::RCTree::graph_type::NodeIt it(tree.g()); it != lemon::INVALID; ++it)
        {
            delays[it] = delay.at(it);
            auto step_slew = units::math::sqrt(second_moment.at(it) * 2.0 - units::math::pow<2>(delay.at(it)));
            slews[it] = units::math::sqrt(units::math::pow<2>(source_slew) + units::math::pow<2>(step_slew));
        }

        return lumped;
    }

private:
    bool                     m_slews_owner;
    bool                     m_delays_owner;
    bool                     m_ceff_owner;
    slew_mapper_type*        m_slews;
    slew_mapper_type*        m_delays;
    capacitance_mapper_type* m_ceff;
};

}   // namespace WireModel
}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_WIREMODELS_H
