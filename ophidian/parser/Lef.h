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
/** @brief Lef reads a .lef file and stores its data.
 *
 * This is an encapsulation of the LEF library made by
 * Cadence Design Systems to present the sites, layers
 * and macros of a circuit
 */
class Lef
{
public:
	/**
	 * A structure to represent a site
	 */
	struct site {
		/**
		 * @brief An enumeration to represent all the possible symmetries in a site.
		 * They are numbered 1, 2 and 4 to enable binary operations to combine
		 * multiple symmetries
		 */
		enum symmetries {
			X=1, Y=2, NINETY=4
		};

		std::string name; ///< Name of the site
		std::string class_; ///< Class of the site
		double x; ///< Width of the site
		double y; ///< Height of the site

		/// Stores the site's symmetry in accordance to the site::symmetries enumeration
		char symmetry {0};
		void setXsymmetry(); ///< Sets the X symmetry bit
		void setYsymmetry(); ///< Sets the Y symmetry bit
		void set90symmetry(); ///< Sets the R90 symmetry bit
		// void addRowPattern(const char* name, int orient);
	};

	/**
	 * A structure to represent a layer
	 */
	struct layer {
		/// An enumeration to represent all the possible directions of a layer
		enum directions {
			NOT_ASSIGNED, HORIZONTAL, VERTICAL
		};
		std::string name; ///< Name of the layer
		std::string type; ///< Type of the layer
		directions direction; ///< Direction of the layer
		double pitch; ///< Pitch of the layer
		double width; ///< Width of the layer
	};

	/**
	 * A structure to represent a rectangle
	 */
	struct rect {
		double xl; ///< x coordinate of the first point
		double yl; ///< y coordinate of the first point
		double xh; ///< x coordinate of the second point
		double yh; ///< y coordinate of the second point
	};

	/**
	 * A structure to represent a port
	 */
	struct port {
		std::vector<std::string> layers; ///< A vector with all the names of all the port layers
		std::vector<rect> rects; ///< A vector with all the rects of the port
	};

	/**
	 * A structure to represent a pin
	 */
	struct pin {
		/// An enumeration to represent all the possible directions of a pin
		enum directions {
			INPUT, OUTPUT, INOUT, NA
		};

		std::string name; ///< The pin name
		directions direction {NA}; ///< The pin's direction in accordance to pin::directions
		std::vector<port> ports; ///< A vector with all the pin ports
	};

	/**
	 * A structure to represent the size of a macro
	 */
	struct macro_size {
		double x; ///< Width of the macro
		double y; ///< Height of the macro
	};

	/**
	 * A structure to represent the foreign property of a macro
	 */
	struct macro_foreign {
		std::string name; ///< Foreign cell name
		double x; ///< Offset in the x coordinate
		double y; ///< Offset in the y coordinate
	};

	/**
	 * A structure to represent the macro obstructions
	 */
	struct obs {
		/// Map with all the rectangles of obstruction
		std::map< std::string, std::vector<rect> > layer2rects;
	};

	/**
	 * A structure to represent a macro
	 */
	struct macro {
		std::string name; ///< Name of the macro
		std::string class_; ///< Class of the macro
		std::vector<pin> pins; ///< Vector with all the macro pins
		macro_foreign foreign; ///< Struct with the foreign propertiy
		macro_size size; ///< Struct with the size
		std::string site; ///< Site name
		obs obses; ///< Struct with the macro rectangle geometry
		macro_size origin; ///< Struct containing the origin property
	};

private:
	LefDefParser::lefiUnits m_units;
	std::vector<site> sites_;
	std::vector<layer> layers_;
	std::vector<macro> macros_;

public:
	/// Constructor.
	/**
	 * Parses a lef file
	 * \param filename path to a lef file. If the file does not exist the LEF
	 * library will generate a segmentation fault
	 */
	Lef(const std::string &filename);
	virtual ~Lef();

	/// Returns the lef sites
	/**
	 * Returns a vector containing all the sites in the lef
	 */
	const std::vector<site> & sites() const
	{
		return sites_;
	}

	/// Returns the lef layers
	/**
	 * Returns a vector containing all the layers in the lef
	 */
	const std::vector<layer> & layers() const
	{
		return layers_;
	}

	/// Returns the lef macros
	/**
	 * Returns a vector containing all the macros in the lef
	 */
	const std::vector<macro> & macros() const
	{
		return macros_;
	}

	/// Returns the database units
	/**
	 * The return of this function is equivalent to one micron
	 */
	double databaseUnits() const {
		return m_units.databaseNumber();
	}
};

class LefParser {
public:
	LefParser();
    ~LefParser();

    Lef * readFile(const std::string & filename);
};

} /* namespace parser */
} /* namespace ophidian */

#endif /* OPHIDIAN_PARSER_LEF_H */
