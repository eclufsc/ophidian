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

#include "../catch.hpp"
#include "grid_3d.h"

TEST_CASE("routing/grid_3d multilayer", "[routing][grid_3d]") {

    int width = 300;
    int height = 200;
    int depth = 10;
    ophidian::routing::grid_3d grid_3d(width, height, depth);

    //checking initial edge demands
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                REQUIRE(grid_3d.get_horizontal_edge_demand(x,y,z) == 0);
            }
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                REQUIRE(grid_3d.get_vertical_edge_demand(x,y,z) == 0);
            }
        }
    }

    //set horizontal edge capacities
    unsigned horizontal_capacity = 10;
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                grid_3d.set_horizontal_edge_capacity(x,y,z,horizontal_capacity);
            }
        }
    }

    //set vertical edge capacities
    unsigned vertical_capacity = 20;
    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                grid_3d.set_vertical_edge_capacity(x,y,z,vertical_capacity);
            }
        }
    }

    //ensure that all edge gaps correct
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == horizontal_capacity);
            }
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == vertical_capacity);
            }
        }
    }

    //increase demand and check gap that all edge gaps correct
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                grid_3d.increment_horizontal_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == horizontal_capacity-1);
            }
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                grid_3d.increment_vertical_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == vertical_capacity-1);
            }
        }
    }

    //increase demand and check if the gaps are equal to zero
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                for(int i = 0; i < horizontal_capacity-1; ++i)
                    grid_3d.increment_horizontal_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == 0);
            }
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                for(int i = 0; i < vertical_capacity-1; ++i)
                    grid_3d.increment_vertical_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == 0);
            }
        }
    }

    //increase demand and check if the gaps are negative
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int z = 0; z < depth; z+=2){
                for(int i = 0; i < 5; ++i)
                    grid_3d.increment_horizontal_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == -5);
            }
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int z = 1; z < depth; z+=2){
                for(int i = 0; i < 5; ++i)
                    grid_3d.increment_vertical_edge_demand(x,y,z);
                REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == -5);
            }
        }
    }

    //test max and total edge overflow
    ophidian::routing::edge_overflow_calculator edge_overflow_calculator(grid_3d);
    int max_overflow, total_overflow;
    edge_overflow_calculator.get_max_and_total_edges_overflow(max_overflow, total_overflow);
    REQUIRE(max_overflow == 5);
    int expected_total_overflow = (5*(height-1)*width*(depth/2)) + (5*(height)*(width-1)*(depth/2));
    REQUIRE(total_overflow == expected_total_overflow);
}

TEST_CASE("routing/grid_3d singlelayer", "[routing][grid_3d]") {

    int width = 300;
    int height = 200;
    int depth = 1;
    ophidian::routing::grid_3d grid_3d(width, height, depth);

    int z = 0;
    //checking initial edge demands
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            REQUIRE(grid_3d.get_horizontal_edge_demand(x,y,z) == 0);
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            REQUIRE(grid_3d.get_vertical_edge_demand(x,y,z) == 0);
        }
    }

    //set horizontal edge capacities
    unsigned horizontal_capacity = 10;
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            grid_3d.set_horizontal_edge_capacity(x,y,z,horizontal_capacity);
        }
    }

    //set vertical edge capacities
    unsigned vertical_capacity = 20;
    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            grid_3d.set_vertical_edge_capacity(x,y,z,vertical_capacity);
        }
    }

    //ensure that all edge gaps correct
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == horizontal_capacity);
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == vertical_capacity);
        }
    }

    //increase demand and check gap that all edge gaps correct
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            grid_3d.increment_horizontal_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == horizontal_capacity-1);
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            grid_3d.increment_vertical_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == vertical_capacity-1);
        }
    }

    //increase demand and check if the gaps are equal to zero
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int i = 0; i < horizontal_capacity-1; ++i)
                grid_3d.increment_horizontal_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == 0);
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int i = 0; i < vertical_capacity-1; ++i)
                grid_3d.increment_vertical_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == 0);
        }
    }

    //increase demand and check if the gaps are negative
    for(int x = 0; x < width-1; ++x){
        for(int y = 0; y < height; ++y){
            for(int i = 0; i < 5; ++i)
                grid_3d.increment_horizontal_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_horizontal_edge_utilization_gap(x,y,z) == -5);
        }
    }

    for(int x = 0; x < width; ++x){
        for(int y = 0; y < height-1; ++y){
            for(int i = 0; i < 5; ++i)
                grid_3d.increment_vertical_edge_demand(x,y,z);
            REQUIRE(grid_3d.get_vertical_edge_utilization_gap(x,y,z) == -5);
        }
    }

    //test max and total edge overflow
    ophidian::routing::edge_overflow_calculator edge_overflow_calculator(grid_3d);
    int max_overflow, total_overflow;
    edge_overflow_calculator.get_max_and_total_edges_overflow(max_overflow, total_overflow);
    REQUIRE(max_overflow == 5);
    int expected_total_overflow = (5*(height-1)*width) + (5*(height)*(width-1));
    REQUIRE(total_overflow == expected_total_overflow);
}
