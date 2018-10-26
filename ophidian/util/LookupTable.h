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

#ifndef OPHIDIAN_UTIL_LOOKUPTABLE_H
#define OPHIDIAN_UTIL_LOOKUPTABLE_H

#include <vector>

namespace ophidian::util
{
    template <class RowType, class ColumnType, class ValueType>
    struct TableContents
    {
        template <class T> using container_type = std::vector<T>;

        using row_container_type                = container_type<RowType>;
        using column_container_type             = container_type<ColumnType>;
        using value_container_type              = container_type<container_type<ValueType>>;
        row_container_type row_values;
        column_container_type column_values;
        value_container_type values;
    };

    template <class RowType, class ColumnType, class ValueType>
    struct InterpolationStrategy
    {
        static ValueType compute(const RowType & rv, const ColumnType & cv, const TableContents<RowType, ColumnType, ValueType> & c){
            if (c.values.size() == 1)
                if (c.values.front().size() == 1)
                    return c.values.front().front();

            RowType y1, y2;
            ColumnType x1, x2;
            ValueType t[2][2];
            std::size_t row1, row2, column1, column2;

            row1 = c.row_values.size() - 2;
            row2 = c.row_values.size() - 1;
            y1 = c.row_values[row1];
            y2 = c.row_values[row2];

            // loads -- rows
            for (size_t i = 0; i < c.row_values.size() - 1; i++) {
                if (rv >= c.row_values[i] && rv <= c.row_values[i + 1]) {
                    row1 = i;
                    row2 = i + 1;
                    y1 = c.row_values[row1];
                    y2 = c.row_values[row2];
                }
            }

            // transitions -- columns
            if (cv < c.column_values[0]) {
                column1 = 0;
                column2 = 1;
                x1 = c.column_values[column1];
                x2 = c.column_values[column2];
            } else if (cv > c.column_values[c.column_values.size() - 1]) {
                column1 = c.column_values.size() - 2;
                column2 = c.column_values.size() - 1;
                x1 = c.column_values[column1];
                x2 = c.column_values[column2];
            } else {
                for (size_t i = 0; i < c.column_values.size() - 1; i++) {
                    if (cv >= c.column_values[i] && cv <= c.column_values[i + 1]) {
                        column1 = i;
                        column2 = i + 1;
                        x1 = c.column_values[column1];
                        x2 = c.column_values[column2];
                    }
                }
            }

            auto wTransition = (cv - x1) / (x2 - x1);
            auto wLoad = (rv - y1) / (y2 - y1);

            t[0][0] = c.values[row1][column1];
            t[0][1] = c.values[row1][column2];
            t[1][0] = c.values[row2][column1];
            t[1][1] = c.values[row2][column2];

            return ((1 - wTransition) * (1 - wLoad) * t[0][0])
                    + (wTransition * (1 - wLoad) * t[0][1])
                    + ((1 - wTransition) * wLoad * t[1][0])
                    + (wTransition * wLoad * t[1][1]);
        }
    };

    template <class RowType, class ColumnType, class ValueType>
    struct FloorStrategy
    {
        static ValueType compute(const RowType & rv, const ColumnType & cv, const TableContents<RowType, ColumnType, ValueType> & c){

            int row, col = 0;
            for (int i = 0; i < c.row_values.size(); ++i) {
                if(rv > c.row_values[i]){
                    row = i;
                }else{
                    break;
                }
            }

            for (int i = 0; i < c.column_values.size(); ++i) {
                if(rv > c.column_values[i]){
                    col = i;
                }else{
                    break;
                }
            }

            return c.values[row][col];
        }
    };

    template <class RowType, class ColumnType, class ValueType>
    struct CeilingStrategy
    {
        static ValueType compute(const RowType & rv, const ColumnType & cv, const TableContents<RowType, ColumnType, ValueType> & c){

            int row, col = 0;
            for (int i = c.row_values.size() -1; i >= 0; --i) {
                if(rv < c.row_values[i]){
                    row = i;
                }else{
                    break;
                }
            }

            for (int i = c.column_values.size() -1; i >= 0; --i) {
                if(rv < c.column_values[i]){
                    col = i;
                }else{
                    break;
                }
            }

            return c.values[row][col];
        }
    };

    template<class RowType, class ColumnType, class ValueType, class ComputeStrategy>
    class LookupTable{};

    template<class RowType, class ColumnType, class ValueType, template <class, class, class> class ComputeStrategy>
    class LookupTable<RowType, ColumnType, ValueType, ComputeStrategy<RowType, ColumnType, ValueType>>
    {
    public:
        template <class T> using container_type = std::vector<T>;

        using row_container_type                = container_type<RowType>;
        using column_container_type             = container_type<ColumnType>;
        using value_container_type              = container_type<container_type<ValueType>>;
        using contents_type                     = TableContents<RowType, ColumnType, ValueType>;
        using compute_statrgy_type              = ComputeStrategy<RowType, ColumnType, ValueType>;

        //! LookupTable Constructor
        /*!
            \brief Construct the LUT table using the table contents.
            \param lut table contents
        */
        LookupTable(const contents_type & lut) :
            m_contents(lut)
        {}

        LookupTable(contents_type && lut) :
            m_contents{std::move(lut)}
        {}

        //! Default Constructor
        LookupTable() = default;

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
           \brief Equation for access strategy
           \param rv Row index
           \param cv Column index
         */
        ValueType compute(const RowType & rv, const ColumnType & cv) const
        {
            return compute_statrgy_type::compute(rv, cv, m_contents);
        }

        const row_container_type & row_values() const
        {
            return m_contents.row_values;
        }

        const column_container_type & column_values() const
        {
            return m_contents.column_values;
        }

    private:
        contents_type m_contents;
    };
} // namespace ophidian::util

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
