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

    LookupTable()
    {
    }

    LookupTable(const parser::Liberty::LUT & lut) :
        mRowValues(lut.index_1.size()),
        mColumnValues(lut.index_2.size()),
        mValues(lut.values.size())
    {
        for (int i = 0; i < lut.index_1.size(); ++i)
            mRowValues[i] = RowType(lut.index_1[i]);

        for (int i = 0; i < lut.index_2.size(); ++i)
            mColumnValues[i] = ColumnType(lut.index_2[i]);

        for (int i = 0; i < lut.values.size(); ++i)
            for (int j = 0; j < lut.values[0].size(); ++j)
                mValues[i].push_back(ValueType(lut.values[i][j]));
    }

    ~LookupTable()
    {
    }

    ValueType compute(const RowType & rv, const ColumnType & cv) const
    {
        if (mValues.size() == 1)
            if (mValues.front().size() == 1)
                return mValues.front().front();

        RowType y1, y2;
        auto wLoad = RowType(0) / RowType(0);
        ColumnType x1, x2;
        auto wTransition = ColumnType(0) / ColumnType(0);
        ValueType t[2][2];
        std::size_t row1, row2, column1, column2;

        row1 = mRowValues.size() - 2;
        row2 = mRowValues.size() - 1;
        y1 = mRowValues[row1];
        y2 = mRowValues[row2];

        // loads -- rows
        for (size_t i = 0; i < mRowValues.size() - 1; i++) {
            if (rv >= mRowValues[i] && rv <= mRowValues[i + 1]) {
                row1 = i;
                row2 = i + 1;
                y1 = mRowValues[row1];
                y2 = mRowValues[row2];
            }
        }

        // transitions -- columns
        if (cv < mColumnValues[0]) {
            column1 = 0;
            column2 = 1;
            x1 = mColumnValues[column1];
            x2 = mColumnValues[column2];
        } else if (cv > mColumnValues[mColumnValues.size() - 1]) {
            column1 = mColumnValues.size() - 2;
            column2 = mColumnValues.size() - 1;
            x1 = mColumnValues[column1];
            x2 = mColumnValues[column2];
        } else {
            for (size_t i = 0; i < mColumnValues.size() - 1; i++) {
                if (cv >= mColumnValues[i] && cv <= mColumnValues[i + 1]) {
                    column1 = i;
                    column2 = i + 1;
                    x1 = mColumnValues[column1];
                    x2 = mColumnValues[column2];
                }
            }
        }

        //equation for interpolation (Ref - ISPD Contest: http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf), slide 17
        wTransition = (cv - x1) / (x2 - x1);
        wLoad = (rv - y1) / (y2 - y1);

        t[0][0] = mValues[row1][column1];
        t[0][1] = mValues[row1][column2];
        t[1][0] = mValues[row2][column1];
        t[1][1] = mValues[row2][column2];


        return ((1 - wTransition) * (1 - wLoad) * t[0][0])
                + (wTransition * (1 - wLoad) * t[0][1])
                + ((1 - wTransition) * wLoad * t[1][0])
                + (wTransition * wLoad * t[1][1]);
    }

private:
    std::vector<RowType>                mRowValues;
    std::vector<ColumnType>             mColumnValues;
    std::vector<std::vector<ValueType>> mValues;
};

} // namespace timing
} // namespace ophidian

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
