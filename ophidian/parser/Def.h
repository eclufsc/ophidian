/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_PARSER_DEF_H
#define OPHIDIAN_PARSER_DEF_H

#include <string>
#include <vector>
#include <memory>


#include "ParserException.h"

namespace ophidian
{
namespace parser
{

/**
 * This is an encapsulation of a Def object described
 * on a DEF file to present the rows, components,
 * die area and the units of a given circuit in a usable
 * way.
 */
class Def
{
public:
	/**
	 * Type to represent a bidimentional point
	 * @tparam T point coordinate type
	 */
	template <class T>
	struct point
	{
		T x, y;
	};

	/**
	 * @brief Type to represent two oposite @c point<int>
	 *
	 * The points @c lower and @c upper represent the
	 * dimentions and location of an area on a block.
	 */
	struct dieArea
	{
		point<int> lower;
		point<int> upper;
	};

	/**
	 * @brief Type to represent a circuit component.
	 *
	 * This is the data necessary to identify a given
	 * component and it's characteristics.
	 */
	struct component
	{
		std::string name;         ///< Component's name for identification.
		std::string macro;         ///< Component's type, like "NAND2_X1".
		std::string orientation;         ///< Component's orientation, like "N" for north.
		point<int> position;         ///< Component's lower left corner.
		bool fixed;         ///< This determines if the component's position is fixed in space, @c true for fixed.
	};

	/**
	 * @brief Type to represent a circuit row.
	 *
	 * This if the data necessary to identify a given
	 * row and it's characteristics.
	 */
	struct row
	{
		std::string name;         ///< Row's name for identification.
		std::string site;         ///< This is the site to be used by the row defined by a LEF file.
		point<double> origin;         ///< Specifies the location of the first site in the row.
		point<double> step;         ///< Specifies the spacing between sites in horizontal and vertical rows.
		point<double> num;         ///< Specifies the lenght and direction of the row. (x,1) horisontal line of x sites.
	};

	/**
	 * @brief Constructor.
	 *
	 * Empty object.
	 */
	Def();
	~Def();

	/**
	 * Returns the @c dieArea.
	 */
	const dieArea& die() const {
		return mDie;
	}

	/**
	 * Returns a @c std::vector<component> with
	 * all components.
	 */
	const std::vector<component>& components() const {
		return mComponents;
	}

	/**
	 * Returns a @c std::vector<row> with all rows.
	 */
	const std::vector<row>& rows() const {
		return mRows;
	}

	/**
	 * Returns the DEF database units.
	 */
	double database_units() const {
		return mUnits;
	}

private:
	dieArea mDie;
	double mUnits;
	std::vector<component> mComponents;
	std::vector<row> mRows;

public:
	friend class DefParser;
};


/**
 * DefParser uses the DEF lib to read a def file,
 * populating a def object returning a shared_ptr
 * for it.
 */
class DefParser
{
public:
	DefParser();
	~DefParser();

	std::unique_ptr<Def> readFile(const std::string & filename) const throw(InexistentFile);
};
} // namespace parser
} // namespace ophidian

#endif /*OPHIDIAN_PARSER_DEF_H*/
