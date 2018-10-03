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

#ifndef OPHIDIAN_PARSER_LIBERTYPARSER_H
#define OPHIDIAN_PARSER_LIBERTYPARSER_H

// std headers
#include <memory>
#include <vector>
#include <string>

namespace ophidian::parser
{
/** @brief Liberty reads a .lib file and stores its data.
 *
 * This is an encapsulation of the Liberty library made by
 * SYNOPSYS to obtain information referring to a standard cell library
 */
class Liberty{
public:
    /**
     * A structure to represent a LUT information
     */
    struct LUT{
        enum lutInformation{
            CELL_FALL,
            CELL_RISE,
            FALL_TRANSITION,
            RISE_TRANSITION,
            RISE_CONSTRAINT,
            FALL_CONSTRAINT
        };

        lutInformation _lutInformation;
        std::vector<double> index_1;
        std::vector<double> index_2;
        std::vector<std::vector<double>> values;
    };
    /**
     * A structure to represent a Timing information
     */
    struct Timing{
        enum unateness{
            NEGATIVE_UNATE,
            POSITIVE_UNATE,
            NON_UNATE
        };

        enum type{
            COMBINATIONAL,
            RISING_EDGE,
            SETUP_RISING,
            HOLD_RISING
        };

        type timingType;
        unateness timingSense;
        std::string relatedPin;
        std::vector< LUT > luts;
    };
    /**
     * A structure to represent a pin
     */
    struct Pin{
        std::string name;
        double capacitance = 0;
        double maxCapacitance = 0;
        enum directionPin{
            INPUT,
            OUTPUT
        };
        directionPin pinDirection;
        std::vector<Timing> timing;
    };
    /**
     * A structure to represent a cell
     */
    struct Cell{
        std::string name;
        std::vector<Pin> pins;
        bool sequential = false;
    };

public:
    std::string timeUnit = "";
    std::string capacitiveLoadUnit = "";
    double capacitiveLoadUnitValue = 0;
    std::vector<Cell> cells;
};

class LibertyParser
{
public:
    LibertyParser();
    ~LibertyParser();

    std::shared_ptr<Liberty> readFile(const std::string & filename);
private:
    class Pimpl;
    const std::unique_ptr<Pimpl> this_;
};
}

#endif /* OPHIDIAN_PARSER_LIBERTYPARSER_H */
