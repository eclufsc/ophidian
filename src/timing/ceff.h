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
 *
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

namespace ophidian {
namespace timing {

class ceff
{
    using CapacitanceType = boost::units::quantity < boost::units::si::capacitance >;
    using SlewType = boost::units::quantity < boost::units::si::time >;
    double m_precision;

    lemon::ListGraph::NodeMap< SlewType > * m_slews;
    lemon::ListGraph::NodeMap< SlewType > * m_delays;
    lemon::ListGraph::NodeMap< CapacitanceType > * m_ceff;

    bool m_slews_owner;
    bool m_delays_owner;
    bool m_ceff_owner;
public:
    ceff();
    virtual ~ceff();
    void precision(double epsilon);

    void slew_map(lemon::ListGraph::NodeMap< SlewType >& sm);
    void delay_map(lemon::ListGraph::NodeMap< SlewType >& dm);
    void ceff_map(lemon::ListGraph::NodeMap<CapacitanceType> &cm);

    const lemon::ListGraph::NodeMap< SlewType >& slews() const {
        return *m_slews;
    }
    const lemon::ListGraph::NodeMap< SlewType >& delays() const {
        return *m_delays;
    }
    const lemon::ListGraph::NodeMap< CapacitanceType >& ceffs() const {
        return *m_ceff;
    }

    template <class SlewCalculator>
    CapacitanceType simulate(const SlewCalculator & slew_calculator, const interconnection::rc_tree & tree,  const interconnection::rc_tree::capacitor_id source)
    {
        if(!m_slews)
            m_slews = new lemon::ListGraph::NodeMap< SlewType >(tree.graph());
        if(!m_delays)
            m_delays = new lemon::ListGraph::NodeMap< SlewType >(tree.graph());
        if(!m_ceff)
            m_ceff = new lemon::ListGraph::NodeMap< CapacitanceType >(tree.graph());

        lemon::ListGraph::NodeMap< SlewType > & slews = *m_slews;
        lemon::ListGraph::NodeMap< SlewType > & delays = *m_delays;
        lemon::ListGraph::NodeMap< CapacitanceType > & ceff = *m_ceff;

        lemon::ListGraph::NodeMap< lemon::ListGraph::Arc > pred(tree.graph());
        for(lemon::ListGraph::NodeIt it(tree.graph()); it != lemon::INVALID; ++it)
            pred[it] = lemon::INVALID;

        std::deque<lemon::ListGraph::Node> to_process;
        std::vector< lemon::ListGraph::Node > order(tree.capacitor_count());
        order.resize(0);
        to_process.push_back(source);
        while(!to_process.empty())
        {
            auto current = to_process.front();
            to_process.pop_front();
            order.push_back(current);
            for(interconnection::rc_tree::resistor_it it(tree.capacitor_resistors(current)); it != lemon::INVALID; ++it)
            {
                auto opposite_capacitor = tree.other_capacitor(it, current);
                if(opposite_capacitor != source && pred[opposite_capacitor] == lemon::INVALID)
                {
                    pred[opposite_capacitor] = it;
                    to_process.push_back(opposite_capacitor);
                }
            }
        }
        assert(pred[source] == lemon::INVALID);
        for(std::size_t i{0}; i < order.size(); ++i)
        {
            auto current = order[i];
            ceff[current] = tree.capacitance(current);
        }
        for(std::size_t i = order.size()-1; i > 0; --i)
        {
            auto current = order[i];
            auto parent = tree.graph().oppositeNode(current, pred[current]);
            ceff[parent] += ceff[current];
        }


        double error = 1.0;
        std::size_t iteration = 0;

        CapacitanceType current_ceff;
        delays[source] = SlewType(0.0*boost::units::si::seconds);
        while (error > m_precision) {
            current_ceff = ceff[source];
//            std::cout << "iteration #" << iteration++ << " ceff " << current_ceff << std::endl;
            slews[source] = slew_calculator(current_ceff);
            for(std::size_t i = 1; i < order.size(); ++i)
            {
                auto current = order[i];
                auto parent = tree.graph().oppositeNode(order[i], pred[order[i]]);
                auto resistance_with_parent = tree.resistance(pred[order[i]]);

                slews[current] = slews[parent];
                if(slews[parent] > 0.0*boost::units::si::seconds){
                    double x = resistance_with_parent*ceff[current]/slews[parent];
                    slews[current] = slews[parent]/ (1-x*(1-std::exp(-1/x)));
                }
                delays[current] = delays[parent] + resistance_with_parent*ceff[current];
            }
            for(auto node : order)
                ceff[node] = tree.capacitance(node);
            for(auto node_it = order.rbegin(); node_it != order.rend(); ++node_it)
            {
                auto current = *node_it;
                if(pred[*node_it] != lemon::INVALID)
                {
                    auto parent = tree.graph().oppositeNode(current, pred[current]);
                    auto resistance_with_parent = tree.resistance(pred[current]);
                    double x = 2.0 * resistance_with_parent  * ceff[current] / slews[parent];
                    double y = 1.0 - std::exp(-1.0/x);
                    double shielding_factor = (slews[parent] > 0.0*boost::units::si::seconds?1.0 - x * y:1.0);
                    ceff[parent] += shielding_factor*ceff[current];

                }
            }
            error = boost::units::abs(current_ceff-ceff[source])/std::max(current_ceff, ceff[source]);
        }
        return current_ceff;
    }
};

}
}

#endif // OPHIDIAN_TIMING_CEFF_H
