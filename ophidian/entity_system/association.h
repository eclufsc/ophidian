#ifndef OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
#define OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H

#include "attached_property.h"

namespace ophidian {
namespace entity_system {

class association : public attached_property
{
public:
    association(const entity_system & composite, entity_system & component);
    virtual ~association();

    const entity_system& composite() const;
    const entity_system& component() const;
    bool is_component_of(entity_system::entity composite, entity_system::entity component) const;
    bool is_attached(entity_system::entity component) const;
    std::size_t components_size(entity_system::entity composite) const;
    entity_system::entity composite_of(entity_system::entity component) const;
    bounds<std::vector< entity_system::entity >::const_iterator> components_bounds(entity_system::entity composite) const;

    void attach_component(entity_system::entity composite, entity_system::entity component);
    void dettach_component(entity_system::entity composite, entity_system::entity component);
    void on_resize(std::size_t new_size);
    void on_create(entity_system::entity en);

    virtual void on_destroy(entity_system::entity en) = 0;
    virtual void on_clear() = 0;
protected:
    void bare_clear();
    void bare_destroy(entity_system::entity en);
private:
    struct impl;
    std::unique_ptr<impl> this_;
};


template <class Iterator>
void attach_component(association& compo, const Iterator composite_begin, const Iterator composite_end, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_composite(composite_begin);
    Iterator curr_component(component_begin);

    for(; curr_composite != composite_end && curr_component != component_end; ++curr_composite, ++curr_component)
        compo.attach_component(*curr_composite, *curr_component);
}

template <class Iterator>
void attach_component(association& compo, entity_system::entity composite, const Iterator component_begin, const Iterator component_end) {
    Iterator curr_component(component_begin);

    for(;  curr_component != component_end;  ++curr_component)
        compo.attach_component(composite, *curr_component);
}

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_ASSOCIATION_H
