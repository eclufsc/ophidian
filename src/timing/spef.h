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

#include "../interconnection/rc_tree.h"

#ifndef OPHIDIAN_TIMING_SPEF_H
#define OPHIDIAN_TIMING_SPEF_H

namespace ophidian {
namespace timing {

struct spef_tree {
    std::string net_name;
    interconnection::rc_tree tree;
    std::string source;
};

class spef
{
    std::vector<spef_tree> m_trees;

    void tokenize(const std::string & line, std::vector<std::string> & tokens);

    void skip_header(std::istream &in, std::vector<std::string> & tokens);
    bool read_net(std::istream &in, std::vector<std::string> & tokens);

    std::string read_conn(std::istream &in, std::vector<std::string> & tokens, spef_tree & current_tree);
    void read_cap(std::istream &in, std::vector<std::string> & tokens, spef_tree & current_tree);
    void read_res(std::istream &in, std::vector<std::string> & tokens, spef_tree & current_tree);

    bool is_source(const std::vector<std::string> & tokens) const;
public:
    spef();

    void read(const std::string & in);
    void read(std::istream & in);

    const std::vector<spef_tree> & trees() const {
        return m_trees;
    }
};

}
}



#endif // OPHIDIAN_TIMING_SPEF_H
