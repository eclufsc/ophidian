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
    //TODO: Refactor the duplicated codes.
    constexpr uint32_t index(const uint8_t r, const uint8_t c)
    {
        return r*kSize+c;
    }
    constexpr uint8_t size()
    {
        return kSize;
    }
    T value(const T rv, const T cv)
    {
        //loads -- rows
        //transitions -- columns
        if(row.size() == 1 && column.size() == 1)
            return values.at(0);

        assert(rv >= 0);//there is no negative load

        if(cv < 0.0)//if the trasition is too much fast
            return 0.0;

        uint32_t rfirst, rsecond = std::distance(row.begin(), std::lower_bound(row.begin(), row.end(), rv));
        uint32_t cfirst, csecond = std::distance(column.begin(), std::lower_bound(column.begin(), column.end(), cv));

        if(rsecond == 0)//before first element (left extrapolation)
            rsecond = 1;
        else if(rsecond == row.size())//after last element (right extrapolation)
            rsecond = row.size()-1;
        rfirst = rsecond-1;

        if(csecond == 0)//before first element (left extrapolation)
            csecond = 1;
        else if(csecond == column.size())//after last element (right extrapolation)
            csecond = column.size()-1;
        cfirst = csecond-1;

        T wRow = (rv-row[rfirst])/(row[rsecond]-row[rfirst]);
        T wColumn = (cv-column[cfirst])/(column[csecond]-column[cfirst]);
        //equation for interpolation (Ref - ISPD Contest: http://www.ispd.cc/contests/12/ISPD_2012_Contest_Details.pdf), slide 17
        T result = (1.0-wRow)  * (1.0-wColumn) * values[index(rfirst, cfirst)] +
                   (1.0-wRow)  * (wColumn)     * values[index(rfirst, csecond)] +
                    wRow       * (1.0-wColumn) * values[index(rsecond, cfirst)] +
                    wRow       *  wColumn      * values[index(rsecond, csecond)];
        return result;
    }
    std::array<T, kSize> row;
    std::array<T, kSize> column;
    std::array<T, kSize*kSize> values;
};

}
}

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
