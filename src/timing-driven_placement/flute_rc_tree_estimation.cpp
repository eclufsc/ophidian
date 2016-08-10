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

#include "flute_rc_tree_estimation.h"
#include "../interconnection/flute.h"

#include  <boost/geometry/algorithms/equals.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace ophidian {
namespace timingdriven_placement {

using namespace boost::units;

namespace flute_rc_tree_rtree {
typedef std::pair<ophidian::geometry::point<double>, entity_system::entity> rtree_node;

class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>, boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;




}


flute_rc_tree_creator::flute_rc_tree_creator(double max_segment_length)
{
#pragma omp critical
    {
        interconnection::readLUT();
    }
    m_params.max_segment_length = max_segment_length;
    m_params.capacitance_per_micron = quantity<si::capacitance>(1.6e-16 * si::farads);
    m_params.resistance_per_micron = quantity<si::resistance>(2.535 * si::ohms);
}

flute_rc_tree_creator::~flute_rc_tree_creator()
{

}

void flute_rc_tree_creator::resistance_per_microns(boost::units::quantity<si::resistance> resistance)
{
    m_params.resistance_per_micron = resistance;
}

void flute_rc_tree_creator::capacitance_per_micron(boost::units::quantity<si::capacitance> capacitance)
{
    m_params.capacitance_per_micron = capacitance;
}

void flute_rc_tree_creator::max_segment_length(double max_segment_length)
{
    m_params.max_segment_length = max_segment_length;
}

std::unordered_map<entity_system::entity, interconnection::rc_tree::capacitor_id> flute_rc_tree_creator::create_tree(const placement::placement &placement, const entity_system::entity net, interconnection::rc_tree &rc_tree, const timing::library &library)
{
    auto net_pins = placement.netlist().net_pins(net);
    std::unordered_map<entity_system::entity, interconnection::rc_tree::capacitor_id> tap_mapping;

    params dummy{10.0, 0.0*si::ohms, 0.0*si::farads};
    params& param = (placement.netlist().net_name(net)=="iccad_clk"?dummy:m_params);
    //    params & param = m_params;

    std::set< interconnection::rc_tree::capacitor_id > taps;

    if(net_pins.size() == 1)
    {
        auto pin_u = net_pins[0];
        auto u_position = placement.pin_position(pin_u);
        auto u = rc_tree.capacitor_insert("C0");
        auto tap_u = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_u));
        tap_mapping[pin_u] = tap_u;
        taps.insert(tap_u);
        auto pin_cap_u = library.pin_capacitance(placement.netlist().pin_std_cell(pin_u));
        rc_tree.capacitance(tap_u, pin_cap_u);
        rc_tree.resistor_insert(u, tap_u, quantity<si::resistance>(0.0 * si::ohms));

        for(auto t : taps)
            rc_tree.tap_insert(t);

        return tap_mapping;
    }


    if(net_pins.size() == 2)
    {
        auto pin_u = net_pins[0];
        auto pin_v = net_pins[1];
        auto u_position = placement.pin_position(pin_u);
        auto v_position = placement.pin_position(pin_v);
        const interconnection::rc_tree::graph_t::Node CAP_U = rc_tree.capacitor_insert("C0");
        double length = ophidian::geometry::manhattan_distance(u_position, v_position);
        length /= static_cast<double>(placement.lib().dist2microns());

        if(length > 0)
        {

            int number_of_sliced_segments = std::ceil(length / m_params.max_segment_length);

            const interconnection::rc_tree::graph_t::Node CAP_V = rc_tree.capacitor_insert("C1");
            interconnection::rc_tree::graph_t::Node previous = CAP_U;
            double remaining = length;
            for(int j = 0; j < number_of_sliced_segments; ++j)
            {
                double local_length = std::min(remaining, m_params.max_segment_length);

                rc_tree.capacitance(previous, rc_tree.capacitance(previous) + quantity<si::capacitance>((local_length / 2.0) *param.capacitance_per_micron));

                interconnection::rc_tree::graph_t::Node next;
                if(j != number_of_sliced_segments-1)
                    next = rc_tree.capacitor_insert("C_0_" + std::to_string(j));
                else
                    next = CAP_V;

                rc_tree.capacitance(next, rc_tree.capacitance(next) + quantity<si::capacitance>((local_length / 2.0) *param.capacitance_per_micron));
                rc_tree.resistor_insert(previous, next, quantity<si::resistance>(local_length *param.resistance_per_micron));

                previous = next;
                remaining -= local_length;
            }

              auto tap_v = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_v));
              tap_mapping[pin_v] = tap_v;
              taps.insert(tap_v);
              auto pin_cap_v = library.pin_capacitance(placement.netlist().pin_std_cell(pin_v));
              rc_tree.capacitance(tap_v, pin_cap_v);
              rc_tree.resistor_insert(CAP_V, tap_v, quantity<si::resistance>(0.0 * si::ohms));

        } else {

            auto tap_v = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_v));
            tap_mapping[pin_v] = tap_v;
            taps.insert(tap_v);
            auto pin_cap_v = library.pin_capacitance(placement.netlist().pin_std_cell(pin_v));
            rc_tree.capacitance(tap_v, pin_cap_v);
            rc_tree.resistor_insert(CAP_U, tap_v, quantity<si::resistance>(0.0 * si::ohms));
        }



        auto tap_u = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_u));
        tap_mapping[pin_u] = tap_u;
        taps.insert(tap_u);
        auto pin_cap_u = library.pin_capacitance(placement.netlist().pin_std_cell(pin_u));
        rc_tree.capacitance(tap_u, pin_cap_u);
        rc_tree.resistor_insert(CAP_U, tap_u, quantity<si::resistance>(0.0 * si::ohms));

        for(auto t : taps)
            rc_tree.tap_insert(t);

        return tap_mapping;
    }



    std::vector<unsigned> X(net_pins.size());
    std::vector<unsigned> Y(net_pins.size());
    X.resize(0);
    Y.resize(0);
    for (auto p : net_pins) {
        auto position = placement.pin_position(p);
        X.push_back(static_cast<unsigned>(position.x()));
        Y.push_back(static_cast<unsigned>(position.y()));
    }

    auto tree = ophidian::interconnection::flute(net_pins.size(), X.data(), Y.data(), ACCURACY);
    std::size_t num_branches = 2 * tree.deg - 2;

    flute_rc_tree_rtree::rtree tap_indexing;
    std::vector<flute_rc_tree_rtree::rtree_node> nodes(net_pins.size());
    nodes.resize(0);
    for (int i = 0; i < net_pins.size(); ++i)
        nodes.push_back( { geometry::point<double>(X[i], Y[i]), net_pins[i] });
    tap_indexing.insert(nodes.begin(), nodes.end());


    std::vector<bool> tap_created(num_branches, false);


    std::map< std::pair< unsigned, unsigned >, int > branch_map;

    for (std::size_t i { 0 }; i < num_branches; ++i) {
        std::size_t n { static_cast<std::size_t>(tree.branch[i].n) };
        if (n == i)
            continue;

        int from_i = i;
        int to_i = n;

        ophidian::geometry::point<double> from { static_cast<double>(tree.branch[i].x), static_cast<double>(tree.branch[i].y) };
        ophidian::geometry::point<double> to { static_cast<double>(tree.branch[n].x), static_cast<double>(tree.branch[n].y) };

        double length = ophidian::geometry::manhattan_distance(from, to);
        length /= static_cast<double>(placement.lib().dist2microns());

        std::string from_name = "C_" + std::to_string(i);
        auto from_position_pair = std::make_pair(tree.branch[i].x, tree.branch[i].y);
        auto cached = branch_map.find(from_position_pair);
        if(cached != branch_map.end())
        {
            from_i = cached->second;
            from_name = "C_" + std::to_string(from_i);
        }
        else
            branch_map[from_position_pair] = i;

        std::string to_name = "C_" + std::to_string(n);
        auto to_position_pair = std::make_pair(tree.branch[n].x, tree.branch[n].y);
        cached = branch_map.find(to_position_pair);
        if(cached != branch_map.end())
        {
            to_i = cached->second;
            to_name = "C_" + std::to_string(to_i);
        }
        else
            branch_map[to_position_pair] = n;

        // Capacitor U
        const interconnection::rc_tree::graph_t::Node CAP_FROM = rc_tree.capacitor_insert(from_name);

        if(from_i != to_i)
        {
            // Capacitor V
            const interconnection::rc_tree::graph_t::Node CAP_TO = rc_tree.capacitor_insert(to_name);

            int num_PIs = std::ceil(length / m_params.max_segment_length);

            interconnection::rc_tree::graph_t::Node previous = CAP_FROM;
            double remaining = length;
            for(int j = 0; j < num_PIs; ++j)
            {
                double local_length = std::min(remaining, m_params.max_segment_length);

                rc_tree.capacitance(previous, rc_tree.capacitance(previous) + quantity<si::capacitance>((local_length / 2.0) *param.capacitance_per_micron));

                interconnection::rc_tree::graph_t::Node next;
                if(j != num_PIs-1)
                    next = rc_tree.capacitor_insert(from_name + "_" + std::to_string(j));
                else
                    next = CAP_TO;

                rc_tree.capacitance(next, rc_tree.capacitance(next) + quantity<si::capacitance>((local_length / 2.0) *param.capacitance_per_micron));
                rc_tree.resistor_insert(previous, next, quantity<si::resistance>(local_length *param.resistance_per_micron));

                previous = next;
                remaining -= local_length;
            }


            std::vector<flute_rc_tree_rtree::rtree_node> nearest;
            tap_indexing.query(boost::geometry::index::nearest(to, 1), std::back_inserter(nearest));

            bool to_is_tap = !nearest.empty() && boost::geometry::equals(to, nearest.front().first);
            if (to_is_tap && !tap_created[to_i]) {
                entity_system::entity pin { nearest.front().second };

                auto tap_cap = rc_tree.capacitor_insert(placement.netlist().pin_name(pin));

                tap_mapping[pin] = tap_cap;
                rc_tree.tap_insert(tap_cap);

                auto pin_cap = library.pin_capacitance(placement.netlist().pin_std_cell(pin));
                rc_tree.capacitance(tap_cap, pin_cap); // tap pin capacitance
                rc_tree.resistor_insert(CAP_TO, tap_cap, quantity<si::resistance>(0.0 * si::ohms));

                tap_created[to_i]  = true;
            }
        }


        std::vector<flute_rc_tree_rtree::rtree_node> nearest;
        tap_indexing.query(boost::geometry::index::nearest(from, 1), std::back_inserter(nearest));

        bool from_is_tap = !nearest.empty() && boost::geometry::equals(from, nearest.front().first);

        if (from_is_tap && !tap_created[from_i]) {
            entity_system::entity pin { nearest.front().second };

            auto tap_cap = rc_tree.capacitor_insert(placement.netlist().pin_name(pin));

            tap_mapping[pin] = tap_cap;
            rc_tree.tap_insert(tap_cap);

            auto pin_cap = library.pin_capacitance(placement.netlist().pin_std_cell(pin));
            rc_tree.capacitance(tap_cap, pin_cap); // tap pin capacitance
            rc_tree.resistor_insert(CAP_FROM, tap_cap, quantity<si::resistance>(0.0 * si::ohms));

            tap_created[from_i]  = true;
        }

    }

    return tap_mapping;
}

} /* namespace timingdriven_placement */
} /* namespace ophidian */
