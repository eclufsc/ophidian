#ifndef OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H
#define OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H

#include "association.h"

namespace ophidian {
namespace entity_system {

class aggregation : public association
{
public:
    aggregation(const entity_system & composite, entity_system & component);
    virtual ~aggregation();
    void on_destroy(entity_system::entity en);
    void on_clear();
};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_AGGREGATION_H
