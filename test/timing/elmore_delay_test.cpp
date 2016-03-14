#include "../catch.hpp"

#include "../timing/rc_tree.h"
#include "../timing/elmore.h"

#include <boost/units/systems/si/prefixes.hpp>

TEST_CASE("elmore_delay/rc_tree with 1 node", "[timing][rc_tree][elmore]")
{
    openeda::timing::rc_tree tree;
    auto cap = tree.capacitor_insert("cap1");
    openeda::timing::elmore delay(tree, cap);
    REQUIRE( delay.get(cap) == boost::units::quantity<boost::units::si::time>(0.0*boost::units::si::seconds) );
}

TEST_CASE("elmore_delay/rc_tree with 2 node2", "[timing][rc_tree][elmore]")
{
    using namespace boost::units;
    openeda::timing::rc_tree tree;
    auto C0 = tree.capacitor_insert("C0");
    auto C1 = tree.capacitor_insert("C1");
    auto R1 = tree.resistor_insert(C0, C1, quantity<si::resistance>(0.223*si::kilo*si::ohms));
    tree.capacitance(C0, quantity<si::capacitance>(1.1*si::femto*si::farads));
    tree.capacitance(C1, quantity<si::capacitance>(2.2*si::femto*si::farads));
    openeda::timing::elmore delay(tree, C0);
    REQUIRE( delay.get(C1) == quantity<si::time>((0.223*2.2)*si::pico*si::seconds) );
}

TEST_CASE("elmore_delay/simulating tap node", "[timing][rc_tree][elmore]")
{
    using namespace boost::units;
    openeda::timing::rc_tree tree;
    /*

    R1       R2       R3 = 0ohms (virtual resistance)
 _/\/\/\___/\/\/\___/\/\/\__
|        |        |        |
= c0     = c1     = u1:a   = u1:a
|        |        | (int.) | (pin capacitance)
z        z        z        z


    */

    auto C0 = tree.capacitor_insert("C0"); // internal node
    tree.capacitance(C0, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto C1 = tree.capacitor_insert("C1"); // internal node
    tree.capacitance(C1, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto u1_a_tap = tree.capacitor_insert("u1:a_tap"); // tap node
    tree.capacitance(u1_a_tap, quantity<si::capacitance>(0.20*si::femto*si::farads));
    auto u1_a = tree.capacitor_insert("u1:a"); // pin capacitance
    tree.capacitance(u1_a, quantity<si::capacitance>(4.0*si::femto*si::farads));

    auto R1 = tree.resistor_insert(C0, C1, quantity<si::resistance>(0.440*si::kilo*si::ohms)); // internal resistor
    auto R2 = tree.resistor_insert(C1, u1_a_tap, quantity<si::resistance>(0.440*si::kilo*si::ohms)); // internal resistor
    auto R3 = tree.resistor_insert(u1_a_tap, u1_a, quantity<si::resistance>(0.0*si::ohms)); // virtual resistor

    openeda::timing::elmore delay(tree, C0);
    quantity<si::time> golden_delay{tree.resistance(R2)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a))+tree.resistance(R1)*(tree.capacitance(u1_a_tap)+tree.capacitance(u1_a)+tree.capacitance(C1))};
    REQUIRE( delay.get(u1_a) == golden_delay );
}

#include "../interconnection_estimation/flute.h"
#include "../geometry/geometry.h"
#include "../timing/elmore.h"
#include <boost/geometry/algorithms/equals.hpp>

#include <lemon/graph_to_eps.h>

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>




typedef std::pair<openeda::geometry::point<double>, std::string> rtree_node;
class rtree_node_comparator {
public:
    bool operator()(const rtree_node & node1, const rtree_node & node2) const {
        return node1.second == node2.second;
    }
};

typedef boost::geometry::index::rtree<rtree_node,
boost::geometry::index::rstar<16>,
boost::geometry::index::indexable<rtree_node>, rtree_node_comparator> rtree;

TEST_CASE("rc_tree/generating tree from flute", "[timing][rc_tree][elmore][flute]")
{
    using namespace boost::units;
    using namespace boost::units::si;

    std::vector<std::string> names{"u1:o", "u2:a", "u3:b", "u4:a"};
    std::vector<unsigned> X{20, 30, 50, 50};
    std::vector<unsigned> Y{20, 40, 10, 50};

    rtree indexing;
    std::vector<rtree_node> nodes(names.size()-1);
    for(int i = 1; i < names.size(); ++i)
        nodes.push_back({openeda::geometry::point<double>(X[i], Y[i]), names[i]});
    indexing.insert(nodes.begin(), nodes.end());

    openeda::interconnection_estimation::readLUT();
    auto tree = openeda::interconnection_estimation::flute(names.size(), X.data(), Y.data(), ACCURACY);

    std::size_t num_branches = 2*tree.deg-2;

    openeda::timing::rc_tree rc_tree;


    openeda::geometry::point<double> source_coord{static_cast<double>(X[0]), static_cast<double>(Y[0])};

    openeda::timing::rc_tree::capacitor_id source;


//    openeda::timing::rc_tree::graph_t::NodeMap<lemon::dim2::Point<double> > coords(rc_tree.graph());

    for(std::size_t i{0}; i < num_branches; ++i)
    {
        std::size_t n{static_cast<std::size_t>(tree.branch[i].n)};
        if(n == i) continue;

        openeda::geometry::point<double> from{static_cast<double>(tree.branch[i].x), static_cast<double>(tree.branch[i].y)};
        openeda::geometry::point<double> to{static_cast<double>(tree.branch[n].x), static_cast<double>(tree.branch[n].y)};
        double length = openeda::geometry::manhattan_distance(from, to);
        length /= 2000.0; // simulate microns

        // Capacitor U
        auto cap_from = rc_tree.capacitor_insert("C_" + std::to_string(i));
        if(boost::geometry::equals(from, source_coord))
            source = cap_from;
        rc_tree.capacitance(cap_from, quantity<si::capacitance>((length/2.0)*0.20*si::femto*si::farads));

        // Capacitor V
        auto cap_to = rc_tree.capacitor_insert("C_" + std::to_string(n));
        if(boost::geometry::equals(to, source_coord))
            source = cap_to;
        rc_tree.capacitance(cap_to, quantity<si::capacitance>((length/2.0)*0.20*si::femto*si::farads));

        // Resistor
        auto res = rc_tree.resistor_insert(cap_from, cap_to, quantity<si::resistance>(length*0.440*si::kilo*si::ohms));

        std::vector<rtree_node> nearest_from, nearest_to;
        indexing.query(boost::geometry::index::nearest(from, 1), std::back_inserter(nearest_from));
        indexing.query(boost::geometry::index::nearest(to, 1), std::back_inserter(nearest_to));

        bool from_is_tap = !nearest_from.empty() && boost::geometry::equals(from, nearest_from.front().first);
        bool to_is_tap = !nearest_to.empty() && boost::geometry::equals(to, nearest_to.front().first);

        if(from_is_tap || to_is_tap)
        {
            std::string name;
            if(from_is_tap)
                name = nearest_from.front().second;
            if(to_is_tap)
                name = nearest_to.front().second;
            auto tap_cap = rc_tree.capacitor_insert(name);
            rc_tree.capacitance(tap_cap, quantity<si::capacitance>(4.0*si::femto*si::farads)); // tap pin capacitance
            if(from_is_tap)
                rc_tree.resistor_insert( cap_from, tap_cap, quantity<si::resistance>(0.0*si::ohms) );
            if(to_is_tap)
                rc_tree.resistor_insert( cap_to, tap_cap, quantity<si::resistance>(0.0*si::ohms) );
        }
    }
    openeda::timing::elmore delay(rc_tree, source);
    delete[] tree.branch;
}
