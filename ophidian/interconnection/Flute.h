#ifndef FLUTE_H
#define FLUTE_H

#include <memory>

namespace ophidian {
namespace interconnection {

class Flute final
{
public:
    static Flute& instance();
    ~Flute();
private:
    Flute();
    Flute(const Flute& o) = delete;
    Flute& operator=(const Flute& o) = delete;
    struct Impl;
    const std::unique_ptr<Impl> this_;
};

}
}

#endif // FLUTE_H
