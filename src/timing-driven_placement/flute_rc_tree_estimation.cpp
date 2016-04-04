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
typedef std::pair<ophidian::geometry::point<double>, entity::entity> rtree_node;
class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>, boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

}


flute_rc_tree_creator::flute_rc_tree_creator()
{
    interconnection::readLUT();
    m_params.capacitance_per_micron = quantity<si::capacitance>(0.16 * si::femto * si::farads);
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

std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> flute_rc_tree_creator::create_tree(const placement::placement &placement, const entity::entity net, interconnection::rc_tree &rc_tree, const timing::library &library)
{
    auto net_pins = placement.netlist().net_pins(net);
    std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> tap_mapping;

    params dummy{0.0*si::ohms, 0.0*si::farads};
    params& param = (placement.netlist().net_name(net)=="iccad_clk"?dummy:m_params);

    if(net_pins.size() == 1)
    {
        auto pin_u = net_pins[0];
        auto u_position = placement.pin_position(pin_u);
        auto u = rc_tree.capacitor_insert("C0");
        auto tap_u = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_u));
        tap_mapping[pin_u] = tap_u;
        auto pin_cap_u = library.pin_capacitance(placement.netlist().pin_std_cell(pin_u));
        rc_tree.capacitance(tap_u, pin_cap_u);
        rc_tree.resistor_insert(u, tap_u, quantity<si::resistance>(0.0 * si::ohms));
        return tap_mapping;
    }


    if(net_pins.size() == 2)
    {
        auto pin_u = net_pins[0];
        auto pin_v = net_pins[1];
        auto u_position = placement.pin_position(pin_u);
        auto v_position = placement.pin_position(pin_v);
        auto u = rc_tree.capacitor_insert("C0");
        auto v = rc_tree.capacitor_insert("C1");
        double length = ophidian::geometry::manhattan_distance(u_position, v_position);
        length /= static_cast<double>(placement.lib().dist2microns());
        rc_tree.capacitance(u, quantity<si::capacitance>((length / 2.0) * param.capacitance_per_micron));
        rc_tree.capacitance(v, quantity<si::capacitance>((length / 2.0) * param.capacitance_per_micron));
        auto res = rc_tree.resistor_insert(u, v, quantity<si::resistance>(length * param.resistance_per_micron));
        auto tap_u = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_u));
        auto tap_v = rc_tree.capacitor_insert(placement.netlist().pin_name(pin_v));

        tap_mapping[pin_u] = tap_u;
        tap_mapping[pin_v] = tap_v;

        auto pin_cap_u = library.pin_capacitance(placement.netlist().pin_std_cell(pin_u));
        rc_tree.capacitance(tap_u, pin_cap_u);
        rc_tree.resistor_insert(u, tap_u, quantity<si::resistance>(0.0 * si::ohms));

        auto pin_cap_v = library.pin_capacitance(placement.netlist().pin_std_cell(pin_v));
        rc_tree.capacitance(tap_v, pin_cap_v);
        rc_tree.resistor_insert(v, tap_v, quantity<si::resistance>(0.0 * si::ohms));
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

    flute_rc_tree_rtree::rtree indexing;
    std::vector<flute_rc_tree_rtree::rtree_node> nodes(net_pins.size());
    nodes.resize(0);
    for (int i = 0; i < net_pins.size(); ++i)
        nodes.push_back( { geometry::point<double>(X[i], Y[i]), net_pins[i] });
    indexing.insert(nodes.begin(), nodes.end());


    for (std::size_t i { 0 }; i < num_branches; ++i) {
        std::size_t n { static_cast<std::size_t>(tree.branch[i].n) };
        if (n == i)
            continue;

        ophidian::geometry::point<double> from { static_cast<double>(tree.branch[i].x), static_cast<double>(tree.branch[i].y) };
        ophidian::geometry::point<double> to { static_cast<double>(tree.branch[n].x), static_cast<double>(tree.branch[n].y) };
        double length = ophidian::geometry::manhattan_distance(from, to);
        length /= static_cast<double>(placement.lib().dist2microns());

        // Capacitor U
        lemon::ListGraph::Node cap_from = rc_tree.capacitor_insert("C_" + std::to_string(i));
        rc_tree.capacitance(cap_from, rc_tree.capacitance(cap_from) + quantity<si::capacitance>((length / 2.0) *param.capacitance_per_micron));

        // Capacitor V
        lemon::ListGraph::Node cap_to = rc_tree.capacitor_insert("C_" + std::to_string(n));
        rc_tree.capacitance(cap_to, rc_tree.capacitance(cap_to) + quantity<si::capacitance>((length / 2.0) * param.capacitance_per_micron));

        // Resistor
        auto res = rc_tree.resistor_insert(cap_from, cap_to, quantity<si::resistance>(length *param.resistance_per_micron));

        std::vector<flute_rc_tree_rtree::rtree_node> nearest;
        indexing.query(boost::geometry::index::nearest(from, 1), std::back_inserter(nearest));

        bool from_is_tap = !nearest.empty() && boost::geometry::equals(from, nearest.front().first);

        if (from_is_tap) {
            entity::entity pin { nearest.front().second };

            auto tap_cap = rc_tree.capacitor_insert(placement.netlist().pin_name(pin));

            tap_mapping[pin] = tap_cap;

            auto pin_cap = library.pin_capacitance(placement.netlist().pin_std_cell(pin));
            rc_tree.capacitance(tap_cap, pin_cap); // tap pin capacitance
            if (from_is_tap)
                rc_tree.resistor_insert(cap_from, tap_cap, quantity<si::resistance>(0.0 * si::ohms));
        }
    }
    return tap_mapping;
}

} /* namespace timingdriven_placement */
} /* namespace openeda */

