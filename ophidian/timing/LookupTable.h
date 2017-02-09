#ifndef OPHIDIAN_TIMING_LOOKUPTABLE_H
#define OPHIDIAN_TIMING_LOOKUPTABLE_H

#include <array>
#include <algorithm>

namespace ophidian {
namespace timing {

template <typename T,uint8_t kSize>
struct LookupTable final
{
    static constexpr uint32_t index(const uint8_t r, const uint8_t c)
    {
        return r*kSize+c;
    }
    static constexpr uint8_t size()
    {
        return kSize;
    }
    float value(const T rv, const T cv)
    {
        uint32_t ri = std::distance(row.begin(), std::lower_bound(row.begin(), row.end(), rv));
        uint32_t ci = std::distance(column.begin(), std::lower_bound(column.begin(), column.end(), cv));
        T wRow = (rv-row[ri-1.0])/(row[ri]-rv);
        T wColumn = (cv-column[ci-1.0])/(column[ci]-cv);
        T result = (1.0-wRow)  * (1.0-wColumn) * values[index(ri-1.0, ci-1.0)] +
                       (1.0-wRow)  * (wColumn)     * values[index(ri-1.0, ci)] +
                        wRow       * (1.0-wColumn) * values[index(ri, ci-1.0)] +
                        wRow       *  wColumn      * values[index(ri, ci)];
        return result;
    }
    //TODO: Handle extrapolation and when no interpolation is necessary.
    std::array<T, kSize> row;
    std::array<T, kSize> column;
    std::array<T, kSize*kSize> values;
};

}
}

#endif // OPHIDIAN_TIMING_LOOKUPTABLE_H
