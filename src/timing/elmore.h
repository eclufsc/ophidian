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

#ifndef SRC_TIMING_ELMORE_H_
#define SRC_TIMING_ELMORE_H_

#include "../interconnection/rc_tree.h"
#include <lemon/maps.h>
namespace ophidian {
namespace timing {

class elmore {

    const interconnection::rc_tree& m_tree;
    lemon::ListGraph::NodeMap< boost::units::quantity< boost::units::si::time > > m_elmore_delay;
    lemon::ListGraph::NodeMap< boost::units::quantity< boost::units::si::capacitance > > m_downstream_capacitance;
    lemon::ListGraph::NodeMap< std::pair<interconnection::rc_tree::capacitor_id, interconnection::rc_tree::resistor_id> > m_pred;
    std::vector< interconnection::rc_tree::capacitor_id > m_order;
    interconnection::rc_tree::capacitor_id m_source;
public:
    elmore(const interconnection::rc_tree& tree, interconnection::rc_tree::capacitor_id source);
    virtual ~elmore();

    void update();

    boost::units::quantity<boost::units::si::time> at(interconnection::rc_tree::capacitor_id capacitor) const {
        return m_elmore_delay[capacitor];
    }

    const lemon::ListGraph::NodeMap< std::pair<interconnection::rc_tree::capacitor_id, interconnection::rc_tree::resistor_id> > & pred() const {
        return m_pred;
    }
    const std::vector< interconnection::rc_tree::capacitor_id > & order() const {
        return m_order;
    }
};

class packed_elmore {
    const interconnection::packed_rc_tree * m_tree;
    std::vector<boost::units::quantity< boost::units::si::time > > m_delays;
public:
    packed_elmore();
    virtual ~packed_elmore();
    void tree(const interconnection::packed_rc_tree & tree);

    const boost::units::quantity< boost::units::si::time >& at(std::size_t i) const {
        return m_delays.at(i);
    }
    void run();
};


} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_ELMORE_H_ */
