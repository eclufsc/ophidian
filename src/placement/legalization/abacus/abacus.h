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

#ifndef OPHIDIAN_ABACUS_H
#define OPHIDIAN_ABACUS_H

#include <legalization.h>
#include "cells.h"

namespace ophidian {
    namespace placement {
        namespace legalization {
            namespace abacus {
                class cell_comparator {
                private:
                    placement * m_placement;
                public:
                    cell_comparator(placement * placement)
                            : m_placement(placement) {

                    }

                    bool operator()(const entity_system::entity cell1, const entity_system::entity cell2) {
                        return m_placement->cell_position(cell1).x() <= m_placement->cell_position(cell2).x();
                    }
                };

                class abacus : public legalization {
                    entity_system::entity_system m_cells_system;
                    cells m_cells;
                public:
                    abacus(floorplan::floorplan *floorplan, placement *placement)
                            : legalization(floorplan, placement), m_cells(m_cells_system) { }

                    void legalize_placement();
                };
            }
        }
    }
}
class abacus {

};


#endif //OPHIDIAN_ABACUS_H
