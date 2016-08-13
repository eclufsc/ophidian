#ifndef OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
#define OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H

#include "association.h"

namespace ophidian {
namespace entity_system {

class composition : public association
{
public:
    composition(const entity_system & composite, entity_system & component);
    virtual ~composition();
    void on_destroy(entity_system::entity en);
    void on_clear();
private:
    struct impl;
    std::unique_ptr<impl> this_;
};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_COMPOSITION_H
