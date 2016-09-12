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

#ifndef OPHIDIAN_PARSER_LEF_H
#define OPHIDIAN_PARSER_LEF_H

#include <string>
#include <vector>
#include <map>
#include <LEF/lefrReader.hpp>

namespace ophidian {
    namespace parser {
        class lef {
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
                    NOT_ASSIGNED, HORIZONTAL, VERTICAL
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
            lef(const std::string & filename);
            virtual ~lef();

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

    } /* namespace parser */
} /* namespace ophidian */

#endif /* OPHIDIAN_PARSER_LEF_H */
