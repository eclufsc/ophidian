/*
 * lookup_table.h
 *
 *  Created on: Mar 16, 2016
 *      Author: csguth
 */

#ifndef SRC_TIMING_LOOKUP_TABLE_H_
#define SRC_TIMING_LOOKUP_TABLE_H_

#include <boost/units/io.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <iostream>

namespace ophidian {
namespace timing {

template<class RowType, class ColumnType, class ValueType>
class lookup_table {
	std::vector<RowType> m_row_values;
	std::vector<ColumnType> m_column_values;
	std::vector<std::vector<ValueType> > m_values;
public:

	bool operator==(const lookup_table<RowType, ColumnType, ValueType> & lut) const
	{
		return (m_row_values == lut.m_row_values)&&(m_column_values==lut.m_column_values)&&(m_values==lut.m_values);
	}

	lookup_table(std::size_t rows=0, std::size_t columns=0) :
			m_row_values(rows), m_column_values(columns), m_values(rows,
					std::vector<ValueType>(columns)) {

	}
	virtual ~lookup_table() {

	}

	std::size_t row_count() const {
		return m_row_values.size();
	}
	void row_value(std::size_t column, RowType value) {
		m_row_values[column] = value;
	}

	RowType row_value(std::size_t column) const {
		return m_row_values[column];
	}

	std::size_t column_count() const {
		return m_column_values.size();
	}

	void column_value(std::size_t row, ColumnType value) {
		m_column_values[row] = value;
	}

	ColumnType column_value(std::size_t row) const {
		return m_column_values[row];
	}

	ValueType at(std::size_t row, std::size_t column) const {
		return m_values.at(row).at(column);
	}

	void at(std::size_t row, std::size_t column, ValueType value) {
		if(row >= m_values.size() || column >= m_values.front().size())
			std::cout << "ERROO!!!" << std::endl;
		m_values.at(row).at(column) = value;
	}

	ValueType compute(RowType rv, ColumnType cv) const {

		if (m_values.size() == 1)
			if (m_values.front().size() == 1)
				return m_values.front().front();

		double wTransition, wLoad;
		RowType y1, y2;

		ColumnType x1, x2;
		ValueType t[2][2];
		std::size_t row1, row2, column1, column2;
		wTransition = 0.0f;
		wLoad = 0.0f;

		row1 = m_row_values.size() - 2;
		row2 = m_row_values.size() - 1;

		y1 = m_row_values[row1];
		y2 = m_row_values[row2];

		// loads -- rows
		for (size_t i = 0; i < m_row_values.size() - 1; i++) {
			if (rv >= m_row_values[i] && rv <= m_row_values[i + 1]) {
				row1 = i;
				row2 = i + 1;
				y1 = m_row_values[row1];
				y2 = m_row_values[row2];
			}
		}

		// transitions -- columns
		if (cv < m_column_values[0]) {
			column1 = 0;
			column2 = 1;
			x1 = m_column_values[column1];
			x2 = m_column_values[column2];
		} else if (cv
				> m_column_values[m_column_values.size() - 1]) {
			column1 = m_column_values.size() - 2;
			column2 = m_column_values.size() - 1;
			x1 = m_column_values[column1];
			x2 = m_column_values[column2];
		} else {
			for (size_t i = 0; i < m_column_values.size() - 1; i++) {
				if (cv >= m_column_values[i]
						&& cv <= m_column_values[i + 1]) {
					column1 = i;
					column2 = i + 1;
					x1 = m_column_values[column1];
					x2 = m_column_values[column2];
				}
			}
		}

		//equation for interpolation (Ref - ISPD Contest: http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf), slide 17
		wTransition = (cv - x1) / (x2 - x1);
		wLoad = (rv - y1) / (y2 - y1);

		t[0][0] = m_values[row1][column1];
		t[0][1] = m_values[row1][column2];
		t[1][0] = m_values[row2][column1];
		t[1][1] = m_values[row2][column2];

		return boost::units::quantity<boost::units::si::time>(((1 - wTransition) * (1 - wLoad) * t[0][0])
				+ (wTransition * (1 - wLoad) * t[0][1])
				+ ((1 - wTransition) * wLoad * t[1][0])
				+ (wTransition * wLoad * t[1][1]));

	}

};

} /* namespace timing */
} /* namespace openeda */

#endif /* SRC_TIMING_LOOKUP_TABLE_H_ */
