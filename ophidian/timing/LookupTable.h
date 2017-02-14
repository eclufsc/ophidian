#ifndef OPHIDIAN_TIMING_LOOKUPTABLE_H
#define OPHIDIAN_TIMING_LOOKUPTABLE_H

#include <array>
#include <algorithm>
#include <cassert>

namespace ophidian {
namespace timing {

template <typename T, uint8_t kSize>
struct LookupTable final
{
    constexpr uint32_t index(const uint8_t r, const uint8_t c) const
    {
        return r*kSize+c;
    }
    constexpr uint8_t size() const
    {
        return kSize;
    }
    /*
     * Find nearest adjacent indexes in a ordered interval of values.
     */
    template <typename Iterator>
    void findNearestIndexes(Iterator begin, Iterator end, const T value, std::pair<uint32_t, uint32_t>& indexes) const {
        indexes.second = std::distance(begin, std::lower_bound(begin, end, value));
        if(indexes.second == 0)//before first element (left extrapolation)
            indexes.second = 1;
        else if(indexes.second == row.size())//after last element (right extrapolation)
            indexes.second = row.size()-1;
        indexes.first = indexes.second-1;
    }
    const T weight(T lower, T upper, T value) const{
        return (value-lower)/(upper-lower);
    }
    const T value(const T rv, const T cv)
    {
        //loads -- rows
        //transitions -- columns
        if(row.size() == 1 && column.size() == 1)
            return values.at(0);

        assert(rv >= 0);//there is no negative load

        if(cv < 0.0)//if the trasition is too much fast
            return 0.0;

        std::pair<uint32_t, uint32_t> rindexes, cindexes;
        findNearestIndexes(row.begin(), row.end(), rv, rindexes);
        findNearestIndexes(column.begin(), column.end(), cv, cindexes);

        T wRow = weight(row[rindexes.first], row[rindexes.second], rv);
        T wColumn = weight(column[cindexes.first], column[cindexes.second], cv);

        //equation for interpolation (Ref - ISPD Contest: http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf), slide 17
        T result = (1.0-wRow)  * (1.0-wColumn) * values[index(rindexes.first, cindexes.first)] +
                   (1.0-wRow)  * (wColumn)     * values[index(rindexes.first, cindexes.second)] +
                    wRow       * (1.0-wColumn) * values[index(rindexes.second, cindexes.first)] +
                    wRow       *  wColumn      * values[index(rindexes.second, cindexes.second)];
        return result;
    }
    std::array<T, kSize> row;
    std::array<T, kSize> column;
    std::array<T, kSize*kSize> values;
};

}
}

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
