#ifndef OPHIDIAN_PARSER_DEF_H
#define OPHIDIAN_PARSER_DEF_H

#include <string>
#include <vector>
#include <def/include/defrReader.hpp>

namespace ophidian {
    namespace parser {

        class Def {
        public:
            template <class T>
            struct point {
                T x, y;
            };

            struct diearea {
                point<int> lower;
                point<int> upper;
            };

            struct component {
                std::string name;
                std::string macro;
                std::string orientation;
                point<int> position;
                bool fixed;
            };

            struct row {
                std::string name;
                std::string site;
                point<double> origin;
                point<double> step;
                point<double> num;
            };

            Def(const std::string& filename);
            ~Def();

            const diearea& die() const {
                return m_die;
            }

            const std::vector<component>& components() const {
                return m_components;
            }

            const std::vector<row>& rows() const {
                return m_rows;
            }

            double database_units() const {
                return m_units;
            }
        private:
            diearea m_die;
            double m_units;
            std::vector<component> m_components;
            std::vector<row> m_rows;
        };

    }
}

#endif /*OPHIDIAN_PARSER_DEF_H*/
