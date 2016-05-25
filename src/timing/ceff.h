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

//
// This is an implementation of the algorithm presented on paper
//    "Fast and accurate wire delay estimation for physical synthesis of large ASICs"
// by Ruchir Puri, David S. Kung, and Anthony D. Drumm.
// published i nProceedings of the 12th ACM Great Lakes symposium on VLSI (GLSVLSI '02).
// ACM, New York, NY, USA, 30-36. DOI=http://dx.doi.org/10.1145/505306.505314
//
//

#ifndef OPHIDIAN_TIMING_CEFF_H
#define OPHIDIAN_TIMING_CEFF_H

#include <deque>
#include <boost/units/cmath.hpp>

#include "../interconnection/rc_tree.h"
#include "elmore.h"
#include "elmore_second_moment.h"

namespace ophidian {
namespace timing {


class lumped_capacitance_wire_model {
    using CapacitanceType = boost::units::quantity < boost::units::si::capacitance >;
    using SlewType = boost::units::quantity < boost::units::si::time >;
    std::vector< SlewType > * m_slews;
    std::vector< SlewType > * m_delays;
    std::vector< CapacitanceType > * m_ceff;
    bool m_slews_owner;
    bool m_delays_owner;
    bool m_ceff_owner;
public:
    lumped_capacitance_wire_model();
    virtual ~lumped_capacitance_wire_model();
    void slew_map(std::vector< SlewType >& sm);
    void delay_map(std::vector< SlewType >& dm);
    void ceff_map(std::vector<CapacitanceType> &cm);
    const std::vector< SlewType >& slews() const {
        return *m_slews;
    }
    const std::vector< SlewType >& delays() const {
        return *m_delays;
    }
    const std::vector< CapacitanceType >& ceffs() const {
        return *m_ceff;
    }
    template <class SlewCalculator>
    CapacitanceType simulate(const SlewCalculator & slew_calculator, const interconnection::packed_rc_tree & tree)
    {
        if(!m_slews)
            m_slews = new std::vector< SlewType >(tree.node_count());
        if(!m_delays)
            m_delays = new std::vector< SlewType >(tree.node_count());
        if(!m_ceff)
            m_ceff = new std::vector< CapacitanceType >(tree.node_count());

        std::vector< SlewType > & slews = *m_slews;
        std::vector< SlewType > & delays = *m_delays;
        std::vector< CapacitanceType > & ceff = *m_ceff;


        CapacitanceType lumped;
        for(std::size_t i = 0; i < tree.node_count(); ++i)
            lumped += tree.capacitance(i);

        auto source_slew = slew_calculator(lumped);

        packed_elmore delay;
        delay.tree(tree);
        delay.run();

        packed_elmore_second_moment second_moment;
        second_moment.elmore(delay);
        second_moment.tree(tree);
        second_moment.run();

        for(std::size_t i = 0; i < tree.node_count(); ++i)
        {
            delays[i] = delay.at(i);
            auto step_slew = boost::units::sqrt( second_moment.at(i)*2.0 - boost::units::pow<2>(delay.at(i)) );
            slews[i] = boost::units::sqrt(boost::units::pow<2>(source_slew) + boost::units::pow<2>(step_slew));
        }

        return lumped;
    }
};


class effective_capacitance_wire_model
{
    using CapacitanceType = boost::units::quantity < boost::units::si::capacitance >;
    using SlewType = boost::units::quantity < boost::units::si::time >;
    double m_precision;

    std::vector< SlewType > * m_slews;
    std::vector< SlewType > * m_delays;
    std::vector< CapacitanceType > * m_ceff;

    bool m_slews_owner;
    bool m_delays_owner;
    bool m_ceff_owner;
public:
    effective_capacitance_wire_model();
    virtual ~effective_capacitance_wire_model();
    void precision(double epsilon);

    void slew_map(std::vector< SlewType >& sm);
    void delay_map(std::vector< SlewType >& dm);
    void ceff_map(std::vector<CapacitanceType> &cm);

    const std::vector< SlewType >& slews() const {
        return *m_slews;
    }
    const std::vector< SlewType >& delays() const {
        return *m_delays;
    }
    const std::vector< CapacitanceType >& ceffs() const {
        return *m_ceff;
    }

    template <class SlewCalculator>
    CapacitanceType simulate(const SlewCalculator & slew_calculator, const interconnection::packed_rc_tree & tree)
    {
        if(!m_slews)
            m_slews = new std::vector< SlewType >(tree.node_count());
        if(!m_delays)
            m_delays = new std::vector< SlewType >(tree.node_count());
        if(!m_ceff)
            m_ceff = new std::vector< CapacitanceType >(tree.node_count());

        std::vector< SlewType > & slews = *m_slews;
        std::vector< SlewType > & delays = *m_delays;
        std::vector< CapacitanceType > & ceff = *m_ceff;



        double error = 1.0;
        std::size_t iteration = 0;

        CapacitanceType current_ceff;
        delays[0] = SlewType(0.0*boost::units::si::seconds);
        while (error > m_precision) {
            current_ceff = ceff[0];
            ////            std::cout << "iteration #" << iteration++ << " ceff " << current_ceff << std::endl;
            slews[0] = slew_calculator(current_ceff);
            for(std::size_t current = 1; current < tree.node_count(); ++current)
            {
                auto parent = tree.pred(current);
                auto resistance_with_parent = tree.resistance(current);

                slews[current] = slews[parent];
                if(slews[parent] > 0.0*boost::units::si::seconds){
                    double x = resistance_with_parent*ceff[current]/slews[parent];
                    slews[current] = slews[parent]/ (1-x*(1-std::exp(-1/x)));
                }
                delays[current] = delays[parent] + resistance_with_parent*ceff[current];
            }
            for(std::size_t node = 0; node < tree.node_count(); ++node)
                ceff[node] = tree.capacitance(node);
            for(std::size_t i = 0; i < tree.node_count()-1; ++i)
            {
                std::size_t current = tree.node_count() - (i+1);
                auto parent = tree.pred(current);
                auto resistance_with_parent = tree.resistance(current);
                double x = 2.0 * resistance_with_parent  * ceff[current] / slews[parent];
                double y = 1.0 - std::exp(-1.0/x);
                double shielding_factor = (slews[parent] > 0.0*boost::units::si::seconds?1.0 - x * y:1.0);
                ceff[parent] += shielding_factor*ceff[current];
            }
            error = boost::units::abs(current_ceff-ceff[0])/std::max(current_ceff, ceff[0]);
        }
        return current_ceff;
    }
};

}
}

#endif // OPHIDIAN_TIMING_CEFF_H
