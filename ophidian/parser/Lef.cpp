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

#include "Lef.h"
#include "ParserException.h"
#include <LEF/include/lefrReader.hpp>
#include <vector>

namespace ophidian
{
namespace parser
{

struct Lef::Impl
{
	std::vector<site> sites_;
	std::vector<layer> layers_;
	std::vector<macro> macros_;
	LefDefParser::lefiUnits units_;
};

const std::vector<Lef::site>& Lef::sites() const {
	return mThis->sites_;
}

const std::vector<Lef::layer>& Lef::layers() const {
	return mThis->layers_;
}

const std::vector<Lef::macro>& Lef::macros() const {
	return mThis->macros_;
}

double Lef::databaseUnits() const {
	return mThis->units_.databaseNumber();
}

LefParser::LefParser() {

}

LefParser::~LefParser() {

}

void LefParser::readFile(const std::string &filename, std::unique_ptr<ophidian::parser::Lef> & inp) {
	auto fp = std::unique_ptr<FILE, decltype(&std::fclose)>(std::fopen(filename.c_str(), "r"), &std::fclose);

	if (!fp)
	{
		throw InexistentFile();
	}

	lefrInit();

//	std::unique_ptr<Lef> inp = std::make_unique<Lef>();

	lefrSetUnitsCbk(
		[](lefrCallbackType_e, lefiUnits* units, lefiUserData ud) -> int {
				static_cast<Lef*>(ud)->mThis->units_ = *units;
				static_cast<Lef*>(ud)->mThis->units_.setDatabase(units->databaseName(), units->databaseNumber());
				return 0;
			}
		);

	lefrSetSiteCbk(
		[](lefrCallbackType_e, lefiSite* l, lefiUserData ud) -> int {
				Lef::site s;
				s.name = l->name();
				s.mClass = (l->hasClass() ? l->siteClass() : "");

				if(l->hasXSymmetry())
					s.setXsymmetry();

				if(l->hasYSymmetry())
					s.setYsymmetry();

				if(l->has90Symmetry())
					s.set90symmetry();

				s.x = l->sizeX();
				s.y = l->sizeY();

				static_cast<Lef*>(ud)->mThis->sites_.push_back(s);
				return 0;
			}
		);

	lefrSetLayerCbk(
		[](lefrCallbackType_e, lefiLayer* l, lefiUserData ud) -> int {
				Lef::layer lay;
				lay.name = l->name();
				lay.type = (l->hasType() ? l->type() : "");
				lay.direction = Lef::layer::NOT_ASSIGNED;

				if(l->hasDirection())
				{
					if(std::string(l->direction()) == "HORIZONTAL" ||
					   std::string(l->direction()) == "horizontal")
					{
						lay.direction = Lef::layer::HORIZONTAL;
					}
					else if(std::string(l->direction()) == "VERTICAL" ||
							std::string(l->direction()) == "vertical")
					{
						lay.direction = Lef::layer::VERTICAL;
					}
				}

				lay.pitch = l->pitch();
				lay.width = l->width();

				static_cast<Lef*>(ud)->mThis->layers_.push_back(lay);
				return 0;
			}
		);

	lefrSetPinCbk(
		[](lefrCallbackType_e, lefiPin* l, lefiUserData ud) -> int {
				Lef::macro& m = static_cast<Lef*>(ud)->mThis->macros_.back();
				Lef::pin p;
				p.name = l->name();

				if(l->hasDirection())
				{
					if(std::string(l->direction()) == "INPUT")
					{
						p.direction = Lef::pin::INPUT;
					}
					else if(std::string(l->direction()) == "OUTPUT")
					{
						p.direction  = Lef::pin::OUTPUT;
					}
				}

				for(int i = 0; i < l->numPorts(); ++i)
				{
					Lef::port pt;
					for(int j = 0; j < l->port(i)->numItems(); ++j)
					{
						switch(l->port(i)->itemType(j))
						{
							case lefiGeomLayerE:
								pt.layers.push_back(l->port(i)->getLayer(j));
								break;
							case lefiGeomRectE:
								Lef::rect r;
								r.firstPoint = util::LocationMicron(l->port(i)->getRect(j)->xl, l->port(i)->getRect(j)->yl);
								r.secondPoint = util::LocationMicron(l->port(i)->getRect(j)->xh, l->port(i)->getRect(j)->yh);
								pt.rects.push_back(r);
								break;
						}
					}
					p.ports.push_back(pt);
				}

				m.pins.push_back(p);
				return 0;
			}
		);

	lefrSetMacroBeginCbk(
		[](lefrCallbackType_e, const char *string, lefiUserData ud) -> int {
				static_cast<Lef*>(ud)->mThis->macros_.push_back(Lef::macro {string});
				return 0;
			}
		);

	lefrSetObstructionCbk(
		[](lefrCallbackType_e, lefiObstruction* l, lefiUserData ud) -> int {
				auto geometries = l->geometries();
				Lef::macro & m = static_cast<Lef*>(ud)->mThis->macros_.back();
				std::string last_layer;
				for(int i = 0; i < geometries->numItems(); ++i)
				{
					switch(geometries->itemType(i))
					{
						case lefiGeomLayerE:
							last_layer = geometries->getLayer(i);
							break;
						case lefiGeomRectE:
							auto geom_rect =  geometries->getRect(i);
							Lef::rect r {util::LocationMicron(geom_rect->xl, geom_rect->yl), util::LocationMicron(geom_rect->xh, geom_rect->yh)};
							m.obses.layer2rects[last_layer].push_back(r);
							break;
					}
				}
				return 0;
			}
		);

	lefrSetMacroCbk(
		[](lefrCallbackType_e, lefiMacro* l, lefiUserData ud) -> int {
				Lef::macro & m = static_cast<Lef*>(ud)->mThis->macros_.back();
				m.name = l->name();
				m.mClass = (l->hasClass() ? l->macroClass() : "");
				m.origin.x = l->originX();
				m.origin.y = l->originY();
				if(l->hasForeign())
				{
					m.foreign.name = l->foreignName();
					m.foreign.x = l->foreignX();
					m.foreign.y = l->foreignY();
				}
				m.size = Lef::macro_size {l->sizeX(), l->sizeY()};
				m.site = (l->hasSiteName() ? l->siteName() : "");
				return 0;
			}
		);


	lefrSetMacroEndCbk(
		[](lefrCallbackType_e, const char *string, lefiUserData) -> int {
				return 0;
			}
		);

	auto res = lefrRead(fp.get(), filename.c_str(), inp.get());

//	return inp;
}

Lef::Lef() : mThis(new Impl) {
}

Lef::~Lef()
{

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

} /* namespace parser */
} /* namespace ophidian */
