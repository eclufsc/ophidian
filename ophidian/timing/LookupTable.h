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

#ifndef OPHIDIAN_TIMING_LOOKUPTABLE_H
#define OPHIDIAN_TIMING_LOOKUPTABLE_H

#include <ophidian/parser/LibertyParser.h>

namespace ophidian
{
namespace timing
{

template<class RowType, class ColumnType, class ValueType>
class LookupTable
{
public:
    template <class T> using container_type = std::vector<T>;

    using lut_type                          = parser::Liberty::LUT;
    using row_container_type                = container_type<RowType>;
    using column_container_type             = container_type<ColumnType>;
    using value_container_type              = container_type<container_type<ValueType>>;

    //! Default Constructor
    LookupTable() = default;

    //! LookupTable Constructor
    /*!
       \brief Converts the LUT table using the scale factors.
       \param lut LookupTable from liberty parser
       \param row_factor Row scale factor
       \param column_factor Column scale factor
       \param value_factor value scale factor
     */
    LookupTable(const lut_type & lut,
                const RowType& row_factor,
                const ColumnType& column_factor,
                const ValueType& value_factor) :
        m_row_values(lut.index_1.size()),
        m_column_values(lut.index_2.size()),
        m_values(lut.values.size())
    {
        for (int i = 0; i < lut.index_1.size(); ++i)
            m_row_values[i] = row_factor * lut.index_1[i];

        for (int i = 0; i < lut.index_2.size(); ++i)
            m_column_values[i] = column_factor * lut.index_2[i];

        for (int i = 0; i < lut.values.size(); ++i)
            for (int j = 0; j < lut.values[0].size(); ++j)
                m_values[i].push_back(value_factor * lut.values[i][j]);
    }

    //! Copy Constructor
    LookupTable(const LookupTable&) = default;
    LookupTable& operator=(const LookupTable&) = default;

    //! Move Constructor
    LookupTable(LookupTable&&) = default;
    LookupTable& operator=(LookupTable&&) = default;

    //! LookupTable Destructor
    /*!
       \brief Destroys the LookupTable object
     */
    ~LookupTable() = default;

    //! Compute slew
    /*!
       \brief Equation for interpolation ISPD Contest 2012 (http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf)
       \param rv Row index
       \param cv Column index
     */
    ValueType compute(const RowType & rv, const ColumnType & cv) const
    {
        if (m_values.size() == 1)
            if (m_values.front().size() == 1)
                return m_values.front().front();

        RowType y1, y2;
        ColumnType x1, x2;
        ValueType t[2][2];
        std::size_t row1, row2, column1, column2;

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
        } else if (cv > m_column_values[m_column_values.size() - 1]) {
            column1 = m_column_values.size() - 2;
            column2 = m_column_values.size() - 1;
            x1 = m_column_values[column1];
            x2 = m_column_values[column2];
        } else {
            for (size_t i = 0; i < m_column_values.size() - 1; i++) {
                if (cv >= m_column_values[i] && cv <= m_column_values[i + 1]) {
                    column1 = i;
                    column2 = i + 1;
                    x1 = m_column_values[column1];
                    x2 = m_column_values[column2];
                }
            }
        }

        auto wTransition = (cv - x1) / (x2 - x1);
        auto wLoad = (rv - y1) / (y2 - y1);

        t[0][0] = m_values[row1][column1];
        t[0][1] = m_values[row1][column2];
        t[1][0] = m_values[row2][column1];
        t[1][1] = m_values[row2][column2];

        return ((1 - wTransition) * (1 - wLoad) * t[0][0])
                + (wTransition * (1 - wLoad) * t[0][1])
                + ((1 - wTransition) * wLoad * t[1][0])
                + (wTransition * wLoad * t[1][1]);
    }

private:
    row_container_type    m_row_values;
    column_container_type m_column_values;
    value_container_type  m_values;
};

} // namespace timing
} // namespace ophidian

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
