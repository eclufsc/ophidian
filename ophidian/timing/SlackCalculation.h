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

#ifndef OPHIDIAN_TIMING_TIMINGCALCULATION_H
#define OPHIDIAN_TIMING_TIMINGCALCULATION_H

#include <ophidian/circuit/Netlist.h>
#include <ophidian/circuit/LibraryMapping.h>
#include <ophidian/timing/Library.h>
#include <ophidian/timing/GenericSTA.h>

namespace ophidian
{
namespace timing
{

class EndPoints
{
public:
    using netlist_type         = circuit::Netlist;
    using library_mapping_type = circuit::LibraryMapping;
    using standard_cells_type  = standard_cell::StandardCells;
    using timing_library_type  = Library;
    using pin_entity_type      = circuit::Pin;
    using pin_container_type   = std::vector<pin_entity_type>;

    //! Default Constructor
    EndPoints() = default;

    //! Copy Constructor
    EndPoints(const EndPoints&) = delete;
    EndPoints& operator=(const EndPoints&) = delete;

    //! Move Constructor
    EndPoints(EndPoints&&) = default;
    EndPoints& operator=(EndPoints&&) = default;

    //! EndPoints Constructor
    /*!
       \brief Constructs the object.
       \param netlist Netlist of the circuit
       \param library_mapping Library Mapping of the circuit
       \param std_cells Standard Cells of the circuit
       \param timing_library Timing library of the circuit
     */
    EndPoints(const netlist_type & netlist,
              const library_mapping_type & library_mapping,
              standard_cells_type & std_cells,
              const timing_library_type & timing_library);

    //! EndPoints Destructor
    /*!
       \brief Destroys the EndPoints object
     */
    ~EndPoints() = default;
    
    //! Initializes Endpoints values
    /*!
       \brief Find the endpoints of the circuit
     */
    void init();

    pin_container_type::const_iterator begin() const;
    pin_container_type::const_iterator end() const;
    std::size_t size() const;
    bool empty() const;

private:
    pin_container_type          m_endpoints;
    const netlist_type&         m_netlist;
    const library_mapping_type& m_library_mapping;
    standard_cells_type&        m_std_cells;
    const timing_library_type&  m_timing_library;
};

class WorstNegativeSlack
{
public:
    using time_unit_type = util::second_t;

    //! GenericSTA Constructor
    /*!
       \brief Constructs the object.
       \param POs Output pins container
       \param sta Generic sta of the circuit
     */
    template <class POsContainer, class WireDelayModel, class MergeStrategy>
    WorstNegativeSlack(const POsContainer& POs, const GenericSTA<WireDelayModel, MergeStrategy> & sta) :
        m_value(std::numeric_limits<time_unit_type>::max())
    {
        for(auto PO : POs)
            m_value = units::math::min(m_value, units::math::min(sta.rise_slack(PO), sta.fall_slack(PO)));
    }

    //! WorstNegativeSlack Destructor
    /*!
       \brief Destroys the WorstNegativeSlack object
     */
    ~WorstNegativeSlack() = default;

    const time_unit_type value() const;

private:
    time_unit_type m_value;
};

class TotalNegativeSlack
{
public:
    using time_unit_type = util::second_t;

    //! GenericSTA Constructor
    /*!
       \brief Constructs the object.
       \param POs Output pins container
       \param sta Generic sta of the circuit
     */
    template <class POsContainer, class WireDelayModel, class MergeStrategy>
    TotalNegativeSlack(const POsContainer& POs, const GenericSTA<WireDelayModel, MergeStrategy> & sta) :
        m_value(0.0)
    {
        for(auto PO : POs)
            m_value += units::math::min(time_unit_type(0), units::math::min(sta.rise_slack(PO), sta.fall_slack(PO)));
    }

    //! TotalNegativeSlack Destructor
    /*!
       \brief Destroys the TotalNegativeSlack object
     */
    ~TotalNegativeSlack() = default;

    const time_unit_type value() const;

private:
    time_unit_type m_value;
};

}   // namespace timing
}   // namespace ophidian

#endif // TIMINGCALCULATION_H
