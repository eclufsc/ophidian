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
#include <LEF/include/lefrReader.hpp>

namespace ophidian
{
namespace parser
{
class Lef
{
public:
	struct site {
		enum symmetries {
			X=1, Y=2, NINETY=4
		};

		std::string name;
		std::string class_;
		double x, y;
		char symmetry {0};
		void setXsymmetry();
		void setYsymmetry();
		void set90symmetry();
		// void addRowPattern(const char* name, int orient);
	};

	/**
	 * A structure to represent a layer
	 */
	struct layer {
		enum directions {
			NOT_ASSIGNED, HORIZONTAL, VERTICAL
		};
		std::string name; //< Name of the layer
		std::string type; //< Type of the layer
		directions direction; //< Direction of the layer
		double pitch; //< Pitch of the layer
		double width; //< Width of the layer
	};

	/**
	 * A structure to represent a rectangle
	 */
	struct rect {
		double xl;
		double yl;
		double xh;
		double yh;
	};

	/**
	 * A structure to represent a port
	 */
	struct port {
		std::vector<std::string> layers; //< A vector with all the names of all the port layers
		std::vector<rect> rects; //< A vector with all the rects of the port
	};

	/**
	 * A structure to represent a pin
	 */
	struct pin {
		enum directions {
			INPUT, OUTPUT, INOUT, NA
		};

		std::string name; //< The pin name
		directions direction {NA}; //< The pin's name
		std::vector<port> ports; //< A vector with all the pin ports
	};

	/**
	 * A structure to represent the size of a macro
	 */
	struct macro_size {
		double x, y;
	};

	/**
	 * A structure to represent the foreign property of a macro
	 */
	struct macro_foreign {
		std::string name;
		double x;
		double y;
	};

	/**
	 * A structure to represent the macro rectancle geometry
	 */
	struct obs {
		std::map< std::string, std::vector<rect> > layer2rects;
	};

	/**
	 * A structure to represent a macro
	 */
	struct macro {
		std::string name; //< Name of the macro
		std::string class_; //< Class of the macro
		std::vector<pin> pins; //< Vector with all the macro pins
		macro_foreign foreign; //< Struct with the foreign propertiy
		macro_size size; //< Struct with the size
		std::string site; //< Site name
		obs obses; //< Struct with the macro rectangle geometry
		macro_size origin; //< Struct containing the origin property
	};

private:
	LefDefParser::lefiUnits m_units;
	std::vector<site> m_sites;
	std::vector<layer> m_layers;
	std::vector<macro> m_macros;

public:
	/// Constructor.
	/**
	 * Parses a lef file
	 * \param filename path to the lef file
	 */
	Lef(const std::string &filename);
	virtual ~Lef();

	/// Returns the lef sites
	/**
	 * Returns a vector containing all the sites in the lef
	 */
	const std::vector<site> & sites() const
	{
		return m_sites;
	}

	/// Returns the lef layers
	/**
	 * Returns a vector containing all the layers in the lef
	 */
	const std::vector<layer> & layers() const
	{
		return m_layers;
	}

	/// Returns the lef macros
	/**
	 * Returns a vector containing all the macros in the lef
	 */
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
