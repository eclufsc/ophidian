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

#include "subrows.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            namespace abacus {

                bool subrows::insert_cell(entity::entity row, entity::entity cell, double cell_width) {
                    if (cell_width <= capacity(row)) {
                        m_cells[m_system.lookup(row)].push_back(cell);
                        m_capacity[m_system.lookup(row)] -= cell_width;
                        return true;
                    }
                    return false;
                }

                void subrows::capacity(entity::entity row, double capacity) {
                    m_capacity[m_system.lookup(row)] = capacity;
                }
            }
        }
    }
}
