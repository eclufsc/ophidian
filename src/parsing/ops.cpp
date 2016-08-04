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

#include "ops.h"
#include <sstream>
#include <fstream>
#include <assert.h>
#include <iostream>

namespace ophidian {
namespace parsing {

void ops::read(std::string filename)
{
    std::fstream in;
    in.open(filename.c_str(), std::fstream::in);
    if (!in.good())
        assert(false);

    std::string pin_name_token;

    std::stringstream ss;

    while (!in.eof())
    {
        std::string connect_or_disconnect_token;
        in >> connect_or_disconnect_token;
        if(connect_or_disconnect_token.empty()) continue;
        if(connect_or_disconnect_token == "disconnect_pin")
        {
            ss.str(std::string());
            ss << connect_or_disconnect_token;
            pin_name_token.clear();
            in >> pin_name_token;
            ss << " " << pin_name_token << std::endl;
            pin_name_token.replace(pin_name_token.find_last_of(":"), 1, "/");
            m_register_pin_to_disconnect.insert(pin_name_token);
        }
        else if(connect_or_disconnect_token == "connect_pin")
        {
            ss << connect_or_disconnect_token;
            std::string lcb_output_pin_net_name;
            in >> lcb_output_pin_net_name;
            ss << " " << lcb_output_pin_net_name;

            lcb_output_pin_net_name.clear();
            in >> lcb_output_pin_net_name;
            ss << " " << lcb_output_pin_net_name << std::endl;

            m_pair_of_register_pin_and_net_to_connect.insert(std::make_pair(pin_name_token, lcb_output_pin_net_name));
        }
        else
        {
            std::cout << ss.str();
            std::cout << "Conn disc token: " << connect_or_disconnect_token << std::endl;
//            assert(false);
        }
    }
}

ops::ops(std::string filename)
{
    read(filename);
}

ops::~ops()
{

}

} /* namespace parsing */
} /* namespace ophidian */

