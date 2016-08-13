#include "association.h"
#include "entity_system.h"

#include "property.h"

#include <algorithm>
#include <functional>

namespace ophidian {
namespace entity_system {

struct part_of_association : public attached_property {
    part_of_association(association &compo, const entity_system & component) :
        m_association(compo),
        m_composites(make_property<entity_system::entity>(component))
    {
        component.notifier()->dettach(m_composites);
        entity_system::null().notifier()->attach(m_composites);
        system(component);
        on_resize(component.size());
    }

    ~part_of_association() {

        entity_system::null().notifier()->dettach(m_composites);

    }

    inline void composite_of(entity_system::entity component, entity_system::entity composite)
    {
        m_composites[component] = composite;
    }

    inline bool is_attached(entity_system::entity en) const {
        return m_composites[en] != entity_system::entity::null();
    }

    inline void on_resize(std::size_t new_size) {
        m_composites.on_resize(new_size);
    }

    inline void on_create(entity_system::entity en) {
        m_composites.on_create(en);
    }

    inline void on_destroy(entity_system::entity en)
    {
        if(is_attached(en))
            m_association.dettach_component(m_composites[en], en);
        m_composites.on_destroy(en);
    }

    inline void on_clear()
    {
        for(auto component : m_association.component())
            if(is_attached(component))
                m_association.dettach_component(m_composites[component], component);
        bare_clear();
    }

    inline entity_system::entity composite_of(entity_system::entity en) const {
        return m_composites[en];
    }

    inline void bare_clear()
    {
        m_composites.on_clear();
    }

    association & m_association;
    property< entity_system::entity > m_composites;

};


// association PIMPL
struct association::impl {
    impl(const entity_system &composite, entity_system &component, association& assoc) :
        m_composite(composite),
        m_component(component),
        m_part(assoc, component),
        m_components(make_property<std::vector<entity_system::entity> >(composite))
    {
        composite.notifier()->dettach(m_components);
        entity_system::null().notifier()->attach(m_components);
    }
    ~impl() {
        entity_system::null().notifier()->dettach(m_components);
    }

    inline const entity_system &composite() const
    {
        return m_composite;
    }

    inline const entity_system &component() const
    {
        return m_component;
    }

    inline bool is_component_of(entity_system::entity composite, entity_system::entity component) const
    {
        return (m_part.composite_of(component) == composite);
    }

    inline void attach_component(entity_system::entity composite, entity_system::entity component)
    {
        m_components[composite].push_back(component);
        m_part.composite_of(component, composite);
    }

    inline void dettach_component(entity_system::entity composite, entity_system::entity component)
    {
        auto& parts = m_components[composite];
        auto erased = std::remove(parts.begin(), parts.end(), component);
        if(erased == parts.end())
            throw std::runtime_error("dettaching unattached component!!");
        parts.erase(erased , parts.end() );
        m_part.composite_of(component, entity_system::entity::null());
    }

    inline bool is_attached(entity_system::entity component) const {
        return m_part.is_attached(component);
    }

    inline bounds<std::vector<entity_system::entity>::const_iterator> components_bounds(entity_system::entity composite) const
    {
        return bounds<std::vector< entity_system::entity >::const_iterator>(m_components.at(composite).begin(), m_components.at(composite).end());
    }

    inline std::size_t components_size(entity_system::entity composite) const {
        return m_components.at(composite).size();
    }

    inline entity_system::entity composite_of(entity_system::entity component) const {
        return m_part.composite_of(component);
    }

    inline void on_resize(std::size_t new_size) {
        m_components.on_resize(new_size);
    }

    inline void on_create(entity_system::entity en) {
        m_components.on_create(en);
    }

    inline void bare_clear() {
        m_components.on_clear();
    }

    inline void bare_destroy(entity_system::entity en) {
        m_components.on_destroy(en);
    }

    inline abstract_property& components() {
        return m_components;
    }

private:
    const entity_system & m_composite;
    entity_system & m_component;
    part_of_association m_part;
    property< std::vector< entity_system::entity > > m_components;
};


// association
association::association(const entity_system &composite, entity_system &component) :
    this_(new impl(composite, component, *this))
{
    system(composite);
    on_resize(composite.size());
}

association::~association()
{
}

const entity_system &association::composite() const
{
    return this_->composite();
}

const entity_system &association::component() const
{
    return this_->component();
}

bool association::is_component_of(entity_system::entity composite, entity_system::entity component) const
{
    return this_->is_component_of(composite, component);
}

void association::attach_component(entity_system::entity composite, entity_system::entity component)
{
    this_->attach_component(composite, component);
}

void association::dettach_component(entity_system::entity composite, entity_system::entity component)
{
    this_->dettach_component(composite, component);
}

bool association::is_attached(entity_system::entity component) const {
    return this_->is_attached(component);
}

bounds<std::vector<entity_system::entity>::const_iterator> association::components_bounds(entity_system::entity composite) const
{
    return this_->components_bounds(composite);
}

std::size_t association::components_size(entity_system::entity composite) const {
    return this_->components_size(composite);
}

entity_system::entity association::composite_of(entity_system::entity component) const {
    return this_->composite_of(component);
}

void association::on_resize(std::size_t new_size) {
    this_->on_resize(new_size);
}

void association::on_create(entity_system::entity en) {
    this_->on_create(en);
}

void association::bare_clear()
{
    this_->bare_clear();
}

void association::bare_destroy(entity_system::entity en)
{
    this_->bare_destroy(en);
}

}
}
