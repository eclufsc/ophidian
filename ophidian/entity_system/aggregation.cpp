#include "aggregation.h"

#include <list>
#include <deque>
#include <cassert>

namespace ophidian {
namespace entity_system {

aggregation::aggregation(const entity_system &composite, entity_system &component) :
    association(composite, component)
{

}

aggregation::~aggregation()
{

}

void aggregation::on_destroy(entity_system::entity en)
{
    auto components = components_bounds(en);
    std::vector<entity_system::entity> local_components(components.begin(), components.end());

    for(auto component : local_components)
        dettach_component(en, component);

    bare_destroy(en);
}

void aggregation::on_clear()
{
    for(auto compo : composite()) {
        auto components = components_bounds(compo);
        std::vector<entity_system::entity> local_components(components.begin(), components.end());
        for(auto component : local_components)
            dettach_component(compo, component);
    }
    bare_clear();
}

}
}
