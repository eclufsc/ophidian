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

#ifndef SRC_PLACEMENT_LEF_H_
#define SRC_PLACEMENT_LEF_H_

#include <string>
#include <vector>
#include "library.h"

#include <LEF/lefrReader.hpp>

namespace ophidian {
namespace placement {

class lef_parser {
public:
    struct site {
        enum symmetries {
            X=1, Y=2, NINETY=4
        };
        std::string name;
        std::string class_;
        double x, y;
        char symmetry{0};
        void setXsymmetry();
        void setYsymmetry();
        void set90symmetry();

        //        void addRowPattern(const char* name, int orient);
    };


    struct layer {
        enum directions {
            HORIZONTAL, VERTICAL
        };
        std::string name;
        std::string type;
        directions direction;
        double pitch;
        double width;
    };

    struct rect {
        double xl;
        double yl;
        double xh;
        double yh;
    };

    struct port {
        std::vector<std::string> layers;
        std::vector<rect> rects;
    };

    struct pin {
        enum directions {
            INPUT, OUTPUT, INOUT, NA
        };

        std::string name;
        directions direction{NA};
        std::vector<port> ports;

    };

    struct macro_size {
        double x, y;
    };

    struct macro_foreign {
        std::string name;
        double x;
        double y;
    };

    struct obs {
        std::map< std::string, std::vector<rect> > layer2rects;
    };

    struct macro {
        std::string name;
        std::string class_;
        std::vector<pin> pins;
        macro_foreign foreign;
        macro_size size;
        std::string site;
        obs obses;
        macro_size origin;

    };



private:


    LefDefParser::lefiUnits m_units;
    std::vector<site> m_sites;
    std::vector<layer> m_layers;
    std::vector<macro> m_macros;

public:
    lef_parser(const std::string & filename);
    virtual ~lef_parser();

    const std::vector<site> & sites() const
    {
        return m_sites;
    }

    const std::vector<layer> & layers() const
    {
        return m_layers;
    }

    const std::vector<macro> & macros() const
    {
        return m_macros;
    }

    double database_units() const {
        return m_units.databaseNumber();
    }

};

//namespace lef {
//namespace parser {

//void get_next_token(std::istream &is, std::string &token,
//                    const char* beginComment);
//void get_next_n_tokens(std::istream &is, std::vector<std::string> &tokens,
//                       const unsigned numTokens, const char* beginComment);

//void read_lef_macro_site(std::istream &is);
//void read_lef_site(std::istream &is);
//void read_lef_layer(std::istream &is);
//void read_lef_macro_pin(std::istream &is,  entity::entity cell, standard_cell::standard_cells * std_cells,
//                        library* lib);
//void read_lef_macro(std::istream &is, standard_cell::standard_cells * std_cells,
//                    library* lib);
//}

//void read(std::istream& dot_lef, standard_cell::standard_cells* std_cells,
//          library* lib);

//}
} /* namespace placement */
} /* namespace ophidian */

#endif /* SRC_PLACEMENT_LEF_H_ */
