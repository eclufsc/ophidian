/*
 *
 * This file is part of Ophidian.
 * Ophidian is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ophidian is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ophidian.  If not, see <http://www.gnu.org/licenses/>.
 *
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
