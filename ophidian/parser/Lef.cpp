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

#include <lefrReader.hpp>

#include "Lef.h"
#include "ParserException.h"

namespace ophidian
{
namespace parser
{
    const std::vector<Lef::site> & Lef::sites() const
    {
        return mSites;
    }

    const std::vector<Lef::layer> & Lef::layers() const
    {
        return mLayers;
    }

    const std::vector<Lef::macro> & Lef::macros() const
    {
        return mMacros;
    }

    util::database_unit_scalar_t Lef::micron_to_dbu_convertion_factor() const
    {
        return util::database_unit_scalar_t{mUnits.databaseNumber()};
    }

    void LefParser::readFile(const std::string & filename, std::unique_ptr<Lef> & inp) const
    {
        auto fp = std::unique_ptr<FILE, decltype( & std::fclose)>(
            std::fopen(filename.c_str(), "r"),
            &std::fclose);

        if(!fp) {
            throw InexistentFile();
        }

        lefrInit();
        lefrSetUnitsCbk(
            [](lefrCallbackType_e, lefiUnits * units, lefiUserData ud) -> int {
                static_cast<Lef *>(ud)->mUnits = *units;
                static_cast<Lef *>(ud)->mUnits.setDatabase(
                    units->databaseName(),
                    units->databaseNumber());

                return 0;
            });

        lefrSetSiteCbk(
            [](lefrCallbackType_e, lefiSite * l, lefiUserData ud) -> int {
                Lef::site s;
                s.name = l->name();
                s.class_name = (l->hasClass() ? l->siteClass() : "");

                if(l->hasXSymmetry()) {
                    s.setXsymmetry();
                }

                if(l->hasYSymmetry()) {
                    s.setYsymmetry();
                }

                if(l->has90Symmetry()) {
                    s.set90symmetry();
                }

                s.x = util::database_unit_t{l->sizeX()};
                s.y = util::database_unit_t{l->sizeY()};

                static_cast<Lef *>(ud)->mSites.push_back(s);

                return 0;
            });

        lefrSetLayerCbk(
            [](lefrCallbackType_e, lefiLayer * l, lefiUserData ud) -> int {
                Lef::layer lay;
                lay.name = l->name();
                lay.type = (l->hasType() ? l->type() : "");
                lay.direction = Lef::layer::NOT_ASSIGNED;

                if(l->hasDirection()) {
                    if((std::string(l->direction()) == "HORIZONTAL") ||
                       (std::string(l->direction()) == "horizontal"))
                    {
                        lay.direction = Lef::layer::HORIZONTAL;
                    }
                    else if((std::string(l->direction()) == "VERTICAL") ||
                            (std::string(l->direction()) == "vertical"))
                    {
                        lay.direction = Lef::layer::VERTICAL;
                    }
                }

                lay.pitch = util::micrometer_t{l->pitch()};
                lay.width = util::micrometer_t{l->width()};

                static_cast<Lef *>(ud)->mLayers.push_back(lay);

                return 0;
            });

        lefrSetPinCbk(
            [](lefrCallbackType_e, lefiPin * l, lefiUserData ud) -> int {
                Lef::macro & m = static_cast<Lef *>(ud)->mMacros.back();
                Lef::macro::pin p;
                p.name = l->name();

                if(l->hasDirection()) {
                    if(std::string(l->direction()) == "INPUT") {
                        p.direction = Lef::macro::pin::INPUT;
                    }
                    else if(std::string(l->direction()) == "OUTPUT") {
                        p.direction = Lef::macro::pin::OUTPUT;
                    }
                }

                for(int i = 0; i < l->numPorts(); ++i)
                {
                    Lef::macro::pin::port pt;
                    for(int j = 0; j < l->port(i)->numItems(); ++j)
                    {
                        switch(l->port(i)->itemType(j))
                        {
                        case lefiGeomLayerE:
                            pt.layers.push_back(l->port(i)->getLayer(j));
                            break;

                        case lefiGeomRectE:
                            geometry::Box<util::micrometer_t> r{
                                geometry::Point<util::micrometer_t>{
                                    util::micrometer_t{l->port(i)->getRect(j)->xl},
                                    util::micrometer_t{l->port(i)->getRect(j)->yl}
                                }, geometry::Point<util::micrometer_t>{
                                    util::micrometer_t{l->port(i)->getRect(j)->xh},
                                    util::micrometer_t{l->port(i)->getRect(j)->yh}
                                }
                            };
                            pt.rects.push_back(r);
                            break;
                        }
                    }
                    p.ports.push_back(pt);
                }

                m.pins.push_back(p);

                return 0;
            });

        lefrSetMacroBeginCbk(
            [](lefrCallbackType_e, const char * string, lefiUserData ud) -> int {
                static_cast<Lef *>(ud)->mMacros.push_back(Lef::macro {string});

                return 0;
            });

        lefrSetObstructionCbk(
            [](lefrCallbackType_e, lefiObstruction * l, lefiUserData ud) -> int {
                auto geometries = l->geometries();
                Lef::macro & m = static_cast<Lef *>(ud)->mMacros.back();
                std::string last_layer;
                for(int i = 0; i < geometries->numItems(); ++i)
                {
                    switch(geometries->itemType(i))
                    {
                    case lefiGeomLayerE:
                        last_layer = geometries->getLayer(i);
                        break;

                    case lefiGeomRectE:
                        auto geom_rect = geometries->getRect(i);
                        geometry::Box<util::micrometer_t> r{
                            geometry::Point<util::micrometer_t>{
                                util::micrometer_t{geom_rect->xl},
                                util::micrometer_t{geom_rect->yl}
                            }, geometry::Point<util::micrometer_t>{
                                util::micrometer_t{geom_rect->xh}, 
                                util::micrometer_t{geom_rect->yh}
                            }
                        };
                        m.obstructions.layer2rects[last_layer].push_back(r);
                        break;
                    }
                }

                return 0;
            });

        lefrSetMacroCbk(
            [](lefrCallbackType_e, lefiMacro * l, lefiUserData ud) -> int {
                Lef::macro & m = static_cast<Lef *>(ud)->mMacros.back();
                m.name = l->name();
                m.class_name = (l->hasClass() ? l->macroClass() : "");
                m.origin = geometry::Point<util::micrometer_t>{
                    util::micrometer_t{l->originX()},
                    util::micrometer_t{l->originY()}
                };
                if(l->hasForeign()) {
                    m.foreign.name = l->foreignName();
                    m.foreign.x = util::micrometer_t{l->foreignX()};
                    m.foreign.y = util::micrometer_t{l->foreignY()};
                }
                m.size = Lef::macro::macro_size {
                    util::micrometer_t{l->sizeX()},
                    util::micrometer_t{l->sizeY()}
                };
                m.site = (l->hasSiteName() ? l->siteName() : "");

                return 0;
            });


        lefrSetMacroEndCbk(
            [](lefrCallbackType_e, const char * string, lefiUserData) -> int {
                return 0;
            });

        auto res = lefrRead(fp.get(), filename.c_str(), inp.get());
    }

    void Lef::site::setXsymmetry()
    {
        symmetry |= X;
    }

    void Lef::site::setYsymmetry()
    {
        symmetry |= Y;
    }

    void Lef::site::set90symmetry()
    {
        symmetry |= NINETY;
    }
}     /* namespace parser */
}     /* namespace ophidian */
