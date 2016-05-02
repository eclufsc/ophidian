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

#ifndef OPHIDIAN_PARSING_VERILOG_H
#define OPHIDIAN_PARSING_VERILOG_H

#include <string>
#include <vector>
#include <utility>

namespace ophidian {
namespace parsing {

class verilog
{
public:
    struct module {
        std::string type;
        std::string name;
        std::vector< std::pair<std::string, std::string> > pinnet_pairs;

        module(){
            pinnet_pairs.reserve(5);
        }

        bool operator==(const module & o) const {
            bool result=pinnet_pairs.size()==o.pinnet_pairs.size();
            if(!result) return false;
            for(std::size_t i=0; i<pinnet_pairs.size();++i)
                if(pinnet_pairs[i].first!=o.pinnet_pairs[i].first||pinnet_pairs[i].second!=o.pinnet_pairs[i].second)
                    return false;
            return type==o.type&&name==o.name;
        }
    };

private:
    std::string m_design;
    std::vector<std::string> m_inputs;
    std::vector<std::string> m_outputs;
    std::vector<std::string> m_wires;
    std::vector<module> m_modules;

    std::size_t m_pin_count;
    std::size_t m_net_count;

    std::vector<std::string> tokenize(std::string line);
    void read(const std::string & filename);



public:
    verilog(const std::string & filename);
    virtual ~verilog();

    const std::string & design() const {
        return m_design;
    }

    const std::vector<std::string> & inputs() const {
        return m_inputs;
    }

    const std::vector<std::string> & outputs() const {
        return m_outputs;
    }

    const std::vector<std::string> & wires() const {
        return m_wires;
    }

    const std::vector<module> & modules() const {
        return m_modules;
    }

    std::size_t cell_count() const {
        return m_modules.size();
    }

    std::size_t net_count() const {
        return m_net_count;
    }

    std::size_t pin_count() const {
        return m_pin_count;
    }

};

}
}

#endif // OPHIDIAN_PARSING_VERILOG_H
