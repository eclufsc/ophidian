#include "Flute.h"
#include <flute/flute.h>

namespace ophidian {
namespace interconnection {

struct Flute::Impl
{

};

Flute::Flute() :
    this_(std::make_unique<Impl>())
{
    readLUT();
}

Flute &Flute::instance()
{
    static Flute instance;
    return instance;
}

Flute::~Flute()
{

}

}
}
