#include "composition.h"
#include "entity_system.h"

#include <algorithm>
#include <functional>
#include <deque>
#include <list>
#include <cassert>

namespace ophidian {
namespace entity_system {

struct composition::impl {
    impl(composition& compo) :
        m_compo(compo){
    }

    ~impl() {

    }

    inline void destroy_component(entity_system::entity en) {
        const_cast<entity_system*>(&m_compo.component())->destroy(en);
    }

private:
    composition& m_compo;
};

composition::composition(const entity_system &composite, entity_system &component) :
    association(composite, component),
    this_(new impl{*this})
{
}

composition::~composition()
{
}

void composition::on_destroy(entity_system::entity en)
{
    while(components_size(en) > 0)
    {
        auto last_component_it = components_bounds(en).end();
        --last_component_it;
        this_->destroy_component(*last_component_it);
    }
    bare_destroy(en);
}

void composition::on_clear()
{
    auto curr = component().begin();
    while(curr != component().end())
    {
        if(is_attached(*curr))
            this_->destroy_component(*curr); // when a element is destroyed, the last element is moved to the current position. thus, we don't need to increment the iterator.
        else
            ++curr;
    }
    bare_clear();
}


}
}
