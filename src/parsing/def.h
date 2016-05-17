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

#ifndef OPHIDIAN_PARSING_DEF_H
#define OPHIDIAN_PARSING_DEF_H

#include <vector>
#include <string>
#include <DEF/defrReader.hpp>

namespace ophidian {
namespace parsing {

class def {
public:
    template <class T>
    struct point {
        T x, y;
    };

    struct diearea {
        point<double> lower;
        point<double> upper;
    };

    struct component {
        std::string name;
        std::string macro;
        std::string orientation;
        point<double> position;
        bool fixed;
    };

    struct row {
        std::string name;
        std::string site;
        point<double> origin;
        point<double> step;
        point<int> num;
    };


private:
    diearea m_die;
    double m_units;
    std::vector<component> m_components;
    std::vector<row> m_rows;


public:
    def(const std::string & filename);
    virtual ~def();

    const diearea & die() const {
        return m_die;
    }

    const std::vector<component> & components() const {
        return m_components;
    }

    const std::vector<row> & rows() const {
        return m_rows;
    }

    double database_units() const {
        return m_units;
    }

};

} /* namespace parsing */
} /* namespace ophidian */

#endif /* OPHIDIAN_PARSING_DEF_H */
