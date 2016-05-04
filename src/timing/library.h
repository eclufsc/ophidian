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

#ifndef SRC_TIMING_LIBRARY_H_
#define SRC_TIMING_LIBRARY_H_

#include "../standard_cell/standard_cells.h"
#include "../entity/vector_property.h"

#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

#include "library_timing_arcs.h"
#include <boost/optional.hpp>

namespace ophidian {
namespace timing {

enum class unateness {
	NEGATIVE_UNATE, POSITIVE_UNATE, NON_UNATE
};

enum class timing_arc_types {
    COMBINATIONAL, SEQUENTIAL, RISING_EDGE
};

class library {
public:
    using CapacitanceType = boost::units::quantity<boost::units::si::capacitance>;
    using SlewType = boost::units::quantity<boost::units::si::time>;
    using LUT = lookup_table<CapacitanceType, SlewType, SlewType>;
    using TestLUT = lookup_table<SlewType, SlewType, SlewType>;

private:
	ophidian::standard_cell::standard_cells & m_std_cells;
	library_timing_arcs & m_tarcs;

	entity::vector_property<LUT> m_rise_delays;
	entity::vector_property<LUT> m_fall_delays;
	entity::vector_property<LUT> m_rise_slews;
	entity::vector_property<LUT> m_fall_slews;
	entity::vector_property<unateness> m_timing_senses;
    entity::vector_property<timing_arc_types> m_timing_types;


    entity::vector_property<boost::optional<std::size_t> > m_arc2setup_rise;
    entity::vector_property<boost::optional<std::size_t> > m_arc2setup_fall;
    entity::vector_property<boost::optional<std::size_t> > m_arc2hold_rise;
    entity::vector_property<boost::optional<std::size_t> > m_arc2hold_fall;
    std::vector<TestLUT> m_tests;

	entity::vector_property<boost::units::quantity<boost::units::si::capacitance> > m_pin_capacitance;

public:
	library(library_timing_arcs * tarcs, ophidian::standard_cell::standard_cells * std_cells);
	virtual ~library();
	void pin_capacitance(entity::entity pin, boost::units::quantity<boost::units::si::capacitance> capacitance);


    void setup_rise_create(entity::entity arc, const TestLUT & lut);
    void hold_rise_create(entity::entity arc, const TestLUT & lut);

    const TestLUT& setup_rise(entity::entity arc) const {
        assert(m_arc2setup_rise[m_tarcs.system().lookup(arc)]);
        return m_tests[*m_arc2setup_rise[m_tarcs.system().lookup(arc)]];
    }

    const TestLUT& hold_rise(entity::entity arc) const {
        assert(m_arc2hold_rise[m_tarcs.system().lookup(arc)]);
        return m_tests[*m_arc2hold_rise[m_tarcs.system().lookup(arc)]];
    }


    void setup_fall_create(entity::entity arc, const TestLUT & lut);
    void hold_fall_create(entity::entity arc, const TestLUT & lut);

    const TestLUT& setup_fall(entity::entity arc) const {
        assert(m_arc2setup_fall[m_tarcs.system().lookup(arc)]);
        return m_tests[*m_arc2setup_fall[m_tarcs.system().lookup(arc)]];
    }

    const TestLUT& hold_fall(entity::entity arc) const {
        assert(m_arc2hold_fall[m_tarcs.system().lookup(arc)]);
        return m_tests[*m_arc2hold_fall[m_tarcs.system().lookup(arc)]];
    }


	boost::units::quantity<boost::units::si::capacitance> pin_capacitance(entity::entity pin) const {
		return m_pin_capacitance[m_std_cells.pin_system().lookup(pin)];
	}
	ophidian::standard_cell::standard_cells & std_cells() {
		return m_std_cells;
	}

    void pin_direction(entity::entity pin, standard_cell::pin_directions direction) {
        return m_std_cells.pin_direction(pin, direction);
    }

	standard_cell::pin_directions pin_direction(entity::entity pin) const {
		return m_std_cells.pin_direction(pin);
	}

	entity::entity timing_arc_from(entity::entity arc) const {
		return m_tarcs.from(arc);
	}

	entity::entity timing_arc_to(entity::entity arc) const {
		return m_tarcs.to(arc);
	}

	entity::entity timing_arc_create(entity::entity from, entity::entity to) {
		return m_tarcs.create(from, to);
	}

	const std::vector<entity::entity>& pin_timing_arcs(entity::entity pin) const {
		return m_tarcs.pin_timing_arcs(pin);
	}

	void timing_arc_rise_slew(entity::entity arc, const LUT & lut);
	void timing_arc_fall_slew(entity::entity arc, const LUT & lut);
	void timing_arc_rise_delay(entity::entity arc, const LUT & lut);
	void timing_arc_fall_delay(entity::entity arc, const LUT & lut);

	const LUT & timing_arc_rise_slew(entity::entity arc) const {
		return m_rise_slews[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_fall_slew(entity::entity arc) const {
		return m_fall_slews[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_rise_delay(entity::entity arc) const {
		return m_rise_delays[m_tarcs.system().lookup(arc)];
	}
	const LUT & timing_arc_fall_delay(entity::entity arc) const {
		return m_fall_delays[m_tarcs.system().lookup(arc)];
	}
	entity::entity timing_arc(entity::entity from, entity::entity to) const {
		return m_tarcs.get(from, to);
	}


	void timing_arc_timing_sense(entity::entity arc, unateness timing_sense);
	unateness timing_arc_timing_sense(entity::entity arc) const {
		return m_timing_senses[m_tarcs.system().lookup(arc)];
	}

    void timing_arc_timing_type(entity::entity arc, timing_arc_types type);
    timing_arc_types timing_arc_timing_type(entity::entity arc) const {
        return m_timing_types[m_tarcs.system().lookup(arc)];
    }


	entity::entity cell_create(std::string name);
	entity::entity pin_create(entity::entity cell, std::string name);


    void cell_sequential(entity::entity cell, bool sequential);

    bool cell_sequential(entity::entity cell) const
     {
        return m_std_cells.cell_sequential(cell);
    }
    void pin_clock_input(entity::entity pin, bool clock_input);

    bool pin_clock_input(entity::entity pin) const {
        return m_std_cells.pin_clock_input(pin);
    }
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_LIBRARY_H_ */
