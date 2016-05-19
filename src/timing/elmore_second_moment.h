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

#ifndef SRC_TIMING_ELMORE_SECOND_MOMENT_H_
#define SRC_TIMING_ELMORE_SECOND_MOMENT_H_

#include "elmore.h"

namespace ophidian {
namespace timing {

typedef boost::units::derived_dimension<boost::units::time_base_dimension,2>::type  squared_time_dimension;
typedef boost::units::unit<squared_time_dimension, boost::units::si::system> second_moment;


class elmore_second_moment {
	const elmore & m_elmore;
	const interconnection::rc_tree & m_tree;
	lemon::ListGraph::NodeMap<boost::units::quantity<second_moment> > m_second_moment;
	void update();

public:
	elmore_second_moment(const interconnection::rc_tree & tree, const elmore & e);
	virtual ~elmore_second_moment();
	boost::units::quantity<second_moment> at(interconnection::rc_tree::capacitor_id capacitor) const {
		return m_second_moment[capacitor];
	}
};


class packed_elmore_second_moment {
    const packed_elmore * m_elmore;
    const interconnection::packed_rc_tree * m_tree;
    std::vector<boost::units::quantity<second_moment> > m_second_moment;
public:
    packed_elmore_second_moment();
    virtual ~packed_elmore_second_moment();
    void elmore(const packed_elmore & elmore);
    void tree(const interconnection::packed_rc_tree & tree);

    const boost::units::quantity<second_moment>& at(std::size_t i) const {
        return m_second_moment.at(i);
    }
    void run();
};

} /* namespace timing */
} /* namespace ophidian */

#endif /* SRC_TIMING_ELMORE_SECOND_MOMENT_H_ */
