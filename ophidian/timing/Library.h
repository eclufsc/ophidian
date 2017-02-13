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

#ifndef OPHIDIAN_TIMING_LIBRARY
#define OPHIDIAN_TIMING_LIBRARY

#include <ophidian/entity_system/Aggregation.h>
#include <ophidian/standard_cell/StandardCells.h>
#include <ophidian/util/Units.h>

namespace ophidian
{
namespace timing
{

enum class Unateness {
    NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE
};

enum class TimingType {
    COMBINATIONAL, SETUP_RISING, HOLD_RISING
};

class Library final
{
public:
    class Timing : public entity_system::EntityBase
    {
    public:
        using entity_system::EntityBase::EntityBase;
    };

    class LookupTable {
        // TODO:
    };

    Library(standard_cell::StandardCells& stdCells);
    ~Library();

    //! Capacitance of a pin (setter)
    /*!
      \brief Sets the capacitance value for a pin
      \param pin A handler for the Pin one want to set the Capacitance.
      \param value The capacitance value.
    */
    void capacitance(const standard_cell::Pin& pin, util::Capacitance value);

    //! Capacitance of a pin
    /*!
      \brief Returns the pin capacitance.
      \param pin A handler for the Pin one want to get the Capacitance.
      \return The capacitance for the pin.
    */
    util::Capacitance capacitance(const standard_cell::Pin& pin) const;

    //! Timing of a pin
    /*!
      \brief Returns a Container Wrapper for the Timing property of a Pin;
      \param pin A handler for the Pin we want to get the Timing property.
      \return Container Wrapper for the Timing of a Pin.
    */
    entity_system::Association<standard_cell::Pin, Timing>::Parts timing(const standard_cell::Pin& pin) const;

    //! Timing of a pin
    /*!
      \brief Creates a timing for a pin.
      \param pin A handler for the Pin one wants to create a Timing.
      \return A handler for the created Timing entity.
    */
    Timing add(Timing, const standard_cell::Pin& pin);

    //! Related pin of a Timing (setter)
    /*!
      \brief Sets the related Pin of a Timing.
      \param timing A handler for the Timing one wants to set the related Pin.
      \param pin A handler for the Pin one wants to set as related Pin for \p timing.
    */
    void relatedPin(const Timing& timing, const standard_cell::Pin& pin);

    //! Related pin of a Timing
    /*!
      \brief Gets the related Pin of a Timing.
      \param timing A handler for the Timing one wants to get the related Pin.
      \return A handler for the related Pin of \p timing.
    */
    standard_cell::Pin relatedPin(const Timing& timing) const;

    //! Timing sense of a Timing (setter)
    /*!
      \brief Sets the timing sense of a Timing.
      \param timing A handler for the Timing one wants to set the timing sense.
      \param sense The desired value (e.g., NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE).
    */
    void timingSense(const Timing& timing, Unateness sense);

    //! Timing sense of a Timing
    /*!
      \brief Gets the timing sense of a Timing.
      \param timing A handler for the Timing one wants to get the timing sense.
      \return The timing sense value (e.g., NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE).
    */
    Unateness timingSense(const Timing& timing) const;

    std::map<std::string, LookupTable>& lookupTables(const Timing& timing);

    void timingType(const Timing& timing, TimingType type);
    TimingType timingType(const Timing& timing) const;


private:
    entity_system::Property<standard_cell::Pin, util::Capacitance> pinCapacitances_;
    entity_system::EntitySystem<Timing> timing_;

    entity_system::Composition<standard_cell::Pin, Timing> pinTiming_;
    entity_system::Property<Timing, standard_cell::Pin> relatedPin_;
    entity_system::Property<Timing, Unateness> timingSense_;

    entity_system::Property<Timing, std::map<std::string, LookupTable> > lookupTables_;
    entity_system::Property<Timing, TimingType> timingType_;
};

}
}

#endif
