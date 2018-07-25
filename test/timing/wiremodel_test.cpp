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

#include <catch.hpp>
#include <functional>
#include <ophidian/timingdriven_placement/RCTree.h>
#include <ophidian/timing/GenericSTA.h>
#include <ophidian/timing/WireModels.h>

using namespace ophidian;

using wiremodel_effective_type = timing::wiremodel::EffectiveCapacitance;
using wiremodel_lumped_type    = timing::wiremodel::LumpedCapacitance;
using tree_type                = timingdriven_placement::RCTree;
using slew_unit_type           = timing::wiremodel::slew_unit_type;
using capacitance_unit_type    = timing::wiremodel::capacitance_unit_type;

namespace
{

class WiremodelFixture
{
public:
    std::function<slew_unit_type(capacitance_unit_type)> m_calculator;

    template<class Value>
    bool diff(const Value &a, const Value &b)
    {
        return units::math::abs(a - b) < Value(1.e-5);
    }

    WiremodelFixture() :
        m_calculator([](capacitance_unit_type) { return slew_unit_type(1.2);})
    {

    }
};
} // namespace

TEST_CASE_METHOD(WiremodelFixture, "Wiremodel: effective capacitance", "[timing][wiremodel]")
{
    CHECK(m_calculator(capacitance_unit_type(1)) == slew_unit_type(1.2));
    CHECK(m_calculator(capacitance_unit_type(2)) == slew_unit_type(1.2));

    tree_type tree;
    auto a = tree.addCapacitor("a");
    auto b = tree.addCapacitor("b");
    auto c = tree.addCapacitor("c");
    auto d = tree.addCapacitor("d");

    tree.source(a);

    tree.capacitance(a, capacitance_unit_type(0));
    tree.capacitance(b, capacitance_unit_type(1.0));
    tree.capacitance(c, capacitance_unit_type(1.5));
    tree.capacitance(d, capacitance_unit_type(0));
    auto ab = tree.addResistor(a, b, ophidian::util::ohm_t(0));
    auto bc = tree.addResistor(b, c, ophidian::util::ohm_t(0.5));
    auto cd = tree.addResistor(c, d, ophidian::util::ohm_t(0));

    CHECK(tree.name(tree.source()) == "a");
    auto it = tree.order().begin();
    CHECK(tree.name(*it) == "a");
    CHECK(tree.name(*(++it)) == "b");
    CHECK(tree.name(*(++it)) == "c");
    CHECK(tree.name(*(++it)) == "d");

    wiremodel_effective_type model;

    CHECK(diff(model.simulate(m_calculator, tree), capacitance_unit_type(1.46749)));
    CHECK(model.slews()[a] == slew_unit_type(1.2));
    CHECK(model.slews()[b] == slew_unit_type(1.2));
    CHECK(diff(model.slews()[c], slew_unit_type(2.39432)));
    CHECK(diff(model.slews()[d], slew_unit_type(2.39432)));
    CHECK(model.delays()[a] == slew_unit_type(0));
    CHECK(model.delays()[b] == slew_unit_type(0));
    CHECK(model.delays()[c] == slew_unit_type(0.75));
    CHECK(model.delays()[d] == slew_unit_type(0.75));
    CHECK(diff(model.ceff()[a], capacitance_unit_type(1.46749)));
    CHECK(diff(model.ceff()[b], capacitance_unit_type(1.46749)));
    CHECK(model.ceff()[c] == capacitance_unit_type(1.5));
    CHECK(model.ceff()[d] == capacitance_unit_type(0));
}

TEST_CASE_METHOD(WiremodelFixture, "Wiremodel: lumped capacitance", "[timing][wiremodel]")
{
    tree_type tree;
    auto a = tree.addCapacitor("a");
    auto b = tree.addCapacitor("b");
    auto c = tree.addCapacitor("c");
    auto d = tree.addCapacitor("d");

    tree.source(a);

    tree.capacitance(a, capacitance_unit_type(0));
    tree.capacitance(b, capacitance_unit_type(1.0));
    tree.capacitance(c, capacitance_unit_type(1.5));
    tree.capacitance(d, capacitance_unit_type(0));
    auto ab = tree.addResistor(a, b, ophidian::util::ohm_t(0));
    auto bc = tree.addResistor(b, c, ophidian::util::ohm_t(0.5));
    auto cd = tree.addResistor(c, d, ophidian::util::ohm_t(0));

    CHECK(tree.name(tree.source()) == "a");
    auto it = tree.order().begin();
    CHECK(tree.name(*it) == "a");
    CHECK(tree.name(*(++it)) == "b");
    CHECK(tree.name(*(++it)) == "c");
    CHECK(tree.name(*(++it)) == "d");

    wiremodel_lumped_type model;

    CHECK(model.simulate(m_calculator, tree) == capacitance_unit_type(2.5));
    CHECK(model.slews()[a] == slew_unit_type(1.2));
    CHECK(model.slews()[b] == slew_unit_type(1.2));
    CHECK(diff(model.slews()[c], slew_unit_type(1.4151)));
    CHECK(diff(model.slews()[d], slew_unit_type(1.4151)));
    CHECK(model.delays()[a] == slew_unit_type(0));
    CHECK(model.delays()[b] == slew_unit_type(0));
    CHECK(model.delays()[c] == slew_unit_type(0.75));
    CHECK(model.delays()[d] == slew_unit_type(0.75));
    CHECK(model.ceff()[a] == capacitance_unit_type(0));
    CHECK(model.ceff()[b] == capacitance_unit_type(0));
    CHECK(model.ceff()[c] == capacitance_unit_type(0));
    CHECK(model.ceff()[d] == capacitance_unit_type(0));
}
