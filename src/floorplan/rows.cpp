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

#include "rows.h"

namespace ophidian {
    namespace floorplan {

        rows::rows(entity::system &system) : m_system(system) {
            m_system.register_property(&m_sites);
            m_system.register_property(&m_number_of_sites);
            m_system.register_property(&m_origins);
        }

        void rows::site(entity::entity row, entity::entity site) {
            m_sites[m_system.lookup(row)] = site;
        }

        void rows::number_of_sites(entity::entity row, unsigned number_of_sites) {
            m_number_of_sites[m_system.lookup(row)] = number_of_sites;
        }

        void rows::origin(entity::entity row, rows::point origin) {
            m_origins[m_system.lookup(row)] = origin;
        }
    }
}
