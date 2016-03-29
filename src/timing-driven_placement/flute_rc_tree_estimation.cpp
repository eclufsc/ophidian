/*
 * flute_rc_tree_estimation.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: csguth
 */

#include "flute_rc_tree_estimation.h"
#include "../interconnection/flute.h"

#include  <boost/geometry/algorithms/equals.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace openeda {
namespace timingdriven_placement {

using namespace boost::units;

namespace flute_rc_tree_rtree {
typedef std::pair<openeda::geometry::point<double>, entity::entity> rtree_node;
class rtree_node_comparator {
public:
	bool operator()(const rtree_node & node1, const rtree_node & node2) const {
		return node1.second == node2.second;
	}
};

typedef boost::geometry::index::rtree<rtree_node, boost::geometry::index::rstar<16>, boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

}

std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> flute_rc_tree(const placement::placement& placement, const entity::entity net, interconnection::rc_tree& rc_tree, const timing::library & library) {

	auto net_pins = placement.netlist().net_pins(net);



	std::vector<unsigned> X(net_pins.size());
	std::vector<unsigned> Y(net_pins.size());
	X.resize(0);
	Y.resize(0);
	for (auto p : net_pins) {
		auto position = placement.pin_position(p);
		X.push_back(static_cast<unsigned>(position.x()));
		Y.push_back(static_cast<unsigned>(position.y()));
	}

	auto tree = openeda::interconnection::flute(net_pins.size(), X.data(), Y.data(), ACCURACY);
	std::size_t num_branches = 2 * tree.deg - 2;

	flute_rc_tree_rtree::rtree indexing;
	std::vector<flute_rc_tree_rtree::rtree_node> nodes(net_pins.size());
	nodes.resize(0);
	for (int i = 0; i < net_pins.size(); ++i)
		nodes.push_back( { geometry::point<double>(X[i], Y[i]), net_pins[i] });
	indexing.insert(nodes.begin(), nodes.end());

	std::unordered_map<entity::entity, interconnection::rc_tree::capacitor_id> tap_mapping;

	for (std::size_t i { 0 }; i < num_branches; ++i) {
		std::size_t n { static_cast<std::size_t>(tree.branch[i].n) };
		if (n == i)
			continue;

		openeda::geometry::point<double> from { static_cast<double>(tree.branch[i].x), static_cast<double>(tree.branch[i].y) };
		openeda::geometry::point<double> to { static_cast<double>(tree.branch[n].x), static_cast<double>(tree.branch[n].y) };
		double length = openeda::geometry::manhattan_distance(from, to);
		length /= static_cast<double>(placement.lib().dist2microns());

		std::cout << "length " << length << std::endl;
		// Capacitor U
		lemon::ListGraph::Node cap_from = rc_tree.capacitor_insert("C_" + std::to_string(i));
		//		        if(boost::geometry::equals(from, source_coord))
		//		            source = cap_from;
		rc_tree.capacitance(cap_from, quantity<si::capacitance>((length / 2.0) * 0.20 * si::femto * si::farads));

		// Capacitor V
		lemon::ListGraph::Node cap_to = rc_tree.capacitor_insert("C_" + std::to_string(n));
		//		        if(boost::geometry::equals(to, source_coord))
		//		            source = cap_to;
		rc_tree.capacitance(cap_to, quantity<si::capacitance>((length / 2.0) * 0.20 * si::femto * si::farads));

		// Resistor
		auto res = rc_tree.resistor_insert(cap_from, cap_to, quantity<si::resistance>(length * 0.440 * si::kilo * si::ohms));

		std::vector<flute_rc_tree_rtree::rtree_node> nearest;
		indexing.query(boost::geometry::index::nearest(from, 1), std::back_inserter(nearest));

		bool from_is_tap = !nearest.empty() && boost::geometry::equals(from, nearest.front().first);

		if (from_is_tap) {
			entity::entity pin { nearest.front().second };

			std::cout << "tap name " << placement.netlist().pin_name(pin) << std::endl;

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

