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

#ifndef OPHIDIAN_TIMING_ELMORE_H
#define OPHIDIAN_TIMING_ELMORE_H

#include <ophidian/util/Units.h>
#include <ophidian/timingdriven_placement/RCTree.h>

namespace ophidian
{
namespace timing
{

class Elmore
{
public:
    using time_unit_type                        = util::second_t;
    using capacitance_unit_type                 = util::farad_t;
    template <class T> using container_type     = std::vector<T>;
    template <class U, class T> using pair_type = std::pair<U, T>;

    using graph_type                            = timingdriven_placement::RCTree::GraphType;
    using capacitor_type                        = timingdriven_placement::RCTree::Capacitor;
    using resistor_type                         = timingdriven_placement::RCTree::Resistor;

    using time_map_type                         = graph_type::NodeMap<time_unit_type>;
    using capacitance_map_type                  = graph_type::NodeMap<capacitance_unit_type>;
    using predecessor_map_type                  = graph_type::NodeMap<pair_type<capacitor_type, resistor_type>>;
    using order_container_type                  = container_type<capacitor_type>;

    Elmore(const timingdriven_placement::RCTree & tree, const capacitor_type & source);
    virtual ~Elmore();

    void update();

    time_unit_type at(const capacitor_type & cap) const;
    const predecessor_map_type & pred() const;
    const order_container_type & order() const;

private:
    const timingdriven_placement::RCTree & m_tree;
    time_map_type                          m_elmore_delay;
    capacitance_map_type                   m_downstream_capacitance;
    predecessor_map_type                   m_pred;
    order_container_type                   m_order;
    const capacitor_type                   m_source;
};

class ElmoreSecondMoment
{
public:
    using square_time_unit = util::square_second_t;

    using graph_type       = timingdriven_placement::RCTree::GraphType;
    using capacitor_type   = timingdriven_placement::RCTree::Capacitor;
    using resistor_type    = timingdriven_placement::RCTree::Resistor;

    ElmoreSecondMoment(const timingdriven_placement::RCTree & tree, const Elmore & e);

    virtual ~ElmoreSecondMoment();

    square_time_unit at(const capacitor_type & capacitor) const;

private:
    using capacitance_time_unit = units::unit_t<units::compound_unit<units::capacitance::farads, units::time::seconds>>;
    using square_time_map_type  = graph_type::NodeMap<square_time_unit>;

    const Elmore&                         m_elmore;
    const timingdriven_placement::RCTree& m_tree;
    square_time_map_type                  m_second_moment;
    void update();
};

}   // namespace timing
}   // namespace ophidian

#endif // OPHIDIAN_TIMING_ELMORE_H

