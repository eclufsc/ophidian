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

#ifndef OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H
#define OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H

#include "generic_sta.h"
#include "endpoints.h"


namespace ophidian {
namespace timing {


using TimeType = boost::units::quantity< boost::units::si::time > ;
using Cell = entity_system::entity;
using Net = entity_system::entity;
using Pin = entity_system::entity;



class static_timing_analysis
{
    const timing::graph * m_timing_graph;
    const entity_system::vector_property< interconnection::packed_rc_tree > * m_rc_trees;
    const library * m_late_lib;
    const library * m_early_lib;
    const netlist::netlist * m_netlist;
    design_constraints m_dc;


    // lazy pointers
    std::unique_ptr<timing_data> m_late;
    std::unique_ptr<timing_data> m_early;
    std::unique_ptr<graph_and_topology> m_topology;
    std::unique_ptr<generic_sta<lumped_capacitance_wire_model, pessimistic> > m_late_sta;
    std::unique_ptr<generic_sta<lumped_capacitance_wire_model, optimistic> > m_early_sta;
    std::unique_ptr<test_calculator> m_test;
    endpoints m_endpoints;
    TimeType m_lwns;
    TimeType m_ewns;
    TimeType m_ltns;
    TimeType m_etns;

    void init_timing_data();
    void propagate_ats();
    void propagate_rts();
    void update_wns_and_tns();
    bool has_timing_data() const {
        assert(m_rc_trees);
        assert(m_timing_graph);
        assert(m_late_lib && m_early_lib);
        assert(m_netlist);
        return m_late_sta && m_early_sta;
    }
public:
    static_timing_analysis();
    void graph(const timing::graph& g);
    void rc_trees(const entity_system::vector_property<interconnection::packed_rc_tree> &trees);
    void late_lib(const library& lib);
    void early_lib(const library& lib);
    void netlist(const netlist::netlist & netlist);
    void set_constraints(const design_constraints & dc);

    void update_timing();


    TimeType late_wns() const {
        return m_lwns;
    }
    TimeType early_wns() const{
        return m_ewns;
    }
    TimeType late_tns() const {
        return m_ltns;
    }
    TimeType early_tns() const{
        return m_etns;
    }

    TimeType early_rise_slack(Pin p) const {
        return m_early_sta->rise_slack(p);
    }
    TimeType early_fall_slack(Pin p) const {
        return m_early_sta->fall_slack(p);
    }
    TimeType late_rise_slack(Pin p) const {
        return m_late_sta->rise_slack(p);
    }
    TimeType late_fall_slack(Pin p) const {
        return m_late_sta->fall_slack(p);
    }

    TimeType early_rise_arrival(Pin p) const {
        return m_early_sta->rise_arrival(p);
    }
    TimeType early_fall_arrival(Pin p) const {
        return m_early_sta->fall_arrival(p);
    }
    TimeType late_rise_arrival(Pin p) const {
        return m_late_sta->rise_arrival(p);
    }
    TimeType late_fall_arrival(Pin p) const {
        return m_late_sta->fall_arrival(p);
    }

    TimeType early_rise_slew(Pin p) const {
        return m_early_sta->rise_slew(p);
    }
    TimeType early_fall_slew(Pin p) const {
        return m_early_sta->fall_slew(p);
    }
    TimeType late_rise_slew(Pin p) const {
        return m_late_sta->rise_slew(p);
    }
    TimeType late_fall_slew(Pin p) const {
        return m_late_sta->fall_slew(p);
    }

    const endpoints & timing_endpoints() const {
        return m_endpoints;
    }

};

}
}


#endif // OPHIDIAN_TIMING_STATIC_TIMING_ANALYSIS_H
