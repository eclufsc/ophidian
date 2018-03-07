#include "rctree_test.h"
#include <catch.hpp>

#include <ophidian/timingdriven_placement/RCTree.h>

using namespace ophidian::timingdriven_placement;

TEST_CASE("RCTree: empty RCTree", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    REQUIRE(tree.size(RCTree::Capacitor()) == 0);
    REQUIRE(tree.size(RCTree::Resistor()) == 0);
    REQUIRE(tree.lumped() == (0.0 * ophidian::util::farad_t()));
}

TEST_CASE("RCTree: add Capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    tree.addCapacitor("cap");
    REQUIRE(tree.size(RCTree::Capacitor()) == 1);
    REQUIRE(tree.size(RCTree::Resistor()) == 0);
    REQUIRE(tree.lumped() == (0.0 * ophidian::util::farad_t()));
}

TEST_CASE("RCTree: add Capacitance of the capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    tree.capacitance(cap, 1.0 * ophidian::util::farad_t());
    REQUIRE(tree.size(RCTree::Capacitor()) == 1);
    REQUIRE(tree.lumped() == (1.0 * ophidian::util::farad_t()));
}

TEST_CASE("RCTree: add the same capacitor twice", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    tree.addCapacitor("cap");
    tree.addCapacitor("cap");
    REQUIRE(tree.size(RCTree::Capacitor()) == 1);
}

TEST_CASE("RCTree: add Resistor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, 1.1 * ophidian::util::ohm_t());
    REQUIRE(tree.size(RCTree::Capacitor()) == 2);
    REQUIRE(tree.size(RCTree::Resistor()) == 1);
    REQUIRE(tree.resistance(res) == 1.1 * ophidian::util::ohm_t());
}

TEST_CASE("RCTree: name of capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    REQUIRE(tree.name(cap) == "cap");
}

TEST_CASE("RCTree: find capacitor by name", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto cap = tree.addCapacitor("cap");
    REQUIRE(tree.capacitor("cap") == cap);
}

TEST_CASE("RCTree: find resistor by two capacitors", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, 1.1 * ophidian::util::ohm_t());
    REQUIRE(tree.resistor(capU, capV) == res);
    REQUIRE(tree.resistor(capV, capU) == res);
    REQUIRE(tree.resistor(capU, RCTree::Capacitor()) == lemon::INVALID);
    REQUIRE(tree.resistor(RCTree::Capacitor(), capV) == lemon::INVALID);
}

TEST_CASE("RCTree: find opposite capacitor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, 1.1 * ophidian::util::ohm_t());
    REQUIRE(tree.oppositeCapacitor(capU, res) == capV);
    REQUIRE(tree.oppositeCapacitor(capV, res) == capU);
}

TEST_CASE("RCTree: resitor iterator", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto res = tree.addResistor(capU, capV, 1.1 * ophidian::util::ohm_t());
    REQUIRE(tree.g().u(tree.resistors(capV)) == tree.g().u(res));
    REQUIRE(tree.g().v(tree.resistors(capV)) == tree.g().v(res));
    REQUIRE(tree.invalid() == lemon::INVALID);
}

TEST_CASE("RCTree: Copy constructor", "[timingdriven_placement][RCTree]")
{
    RCTree tree;
    auto capU = tree.addCapacitor("capU");
    auto capV = tree.addCapacitor("capV");
    auto capT = tree.addCapacitor("capT");
    auto res = tree.addResistor(capU, capV, 1.1 * ophidian::util::ohm_t());
    REQUIRE(tree.size(RCTree::Capacitor()) == 3);
    REQUIRE(tree.size(RCTree::Resistor()) == 1);

    RCTree copy(tree);
    REQUIRE(copy.size(RCTree::Capacitor()) == 3);
    REQUIRE(copy.size(RCTree::Resistor()) == 1);
}
