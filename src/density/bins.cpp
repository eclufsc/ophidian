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

#include "bins.h"


namespace ophidian {
    namespace density {

        void bins::position(entity_system::entity bin, bins::point position) {
            m_positions[m_system.lookup(bin)] = position;
        }

        void bins::dimension(entity_system::entity bin, bins::point dimension) {
            m_dimensions[m_system.lookup(bin)] = dimension;
        }

        void bins::movable_utilization(entity_system::entity bin, double movable_utilization) {
            m_movable_utilization[m_system.lookup(bin)] = movable_utilization;
        }

        void bins::fixed_utilization(entity_system::entity bin, double fixed_utilization) {
            m_fixed_utilization[m_system.lookup(bin)] = fixed_utilization;
        }

        void bins::free_space(entity_system::entity bin, double free_space) {
            m_free_space[m_system.lookup(bin)] = free_space;
        }
    }
}
