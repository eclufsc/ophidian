#ifndef OPHIDIAN_UTIL_RANGE_H
#define OPHIDIAN_UTIL_RANGE_H

namespace ophidian {
namespace util {

template <class IteratorType>
class Range final {
public:
    Range(const IteratorType begin, const IteratorType end) :
        begin_(begin),
        end_(end) {

    }
    ~Range() {

    }
    inline IteratorType begin() const {
        return begin_;
    }
    inline IteratorType end() const {
        return end_;
    }
    inline typename IteratorType::difference_type size() const {
        return std::distance(begin(), end());
    }
    inline bool empty() const {
        return size() == 0;
    }

private:
    const IteratorType begin_;
    const IteratorType end_;
};

}
}

#endif // OPHIDIAN_UTIL_RANGE_H
