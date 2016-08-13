#ifndef OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
#define OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H

#include <cassert>
#include "attached_property.h"

namespace ophidian {
namespace entity_system {

template <class PropertyType>
class property : public attached_property
{
public:

    class helper {
    public:
        static property<PropertyType> make_property(const entity_system & sys) {
            return std::move(property<PropertyType>(sys));
        }
    };

    friend class helper;

    using ContainerType = std::vector<PropertyType>;
    using IteratorType = typename ContainerType::iterator;
    using ConstIteratorType = typename ContainerType::const_iterator;

private:

    ContainerType m_container;

    void init(const entity_system & sys) {
        system(sys);
        on_resize(system().size());
    }

protected:
    property(const entity_system & sys) :
        attached_property()
    {
        init(sys);
    }

public:
    property(const property & prop) :
        property(prop.system())
    {
    }

    property(property &&prop) {
        init(prop.system());
        prop.system().notifier()->dettach(prop);
    }

    property& operator=(const property& prop) {
        init(prop.system());
        return *this;
    }

    property()
    {
    }

    std::size_t lookup(entity_system::entity en) const {
        return system().lookup(en);
    }

    ConstIteratorType begin() const {
        return m_container.cbegin();
    }

    ConstIteratorType end() const {
        return m_container.cend();
    }

    std::size_t size() const {
        return m_container.size();
    }


    void on_create(entity_system::entity en) {
        m_container.resize(m_container.size()+1);
    }

    void on_destroy(entity_system::entity en) {
        std::size_t index = lookup(en);
        if(index != m_container.size() - 1)
            std::swap(m_container.at(lookup(en)), m_container.back());
        m_container.pop_back();
    }

    void on_clear() {
        m_container.clear();
    }

    void on_resize(std::size_t new_size) {
        m_container.resize(new_size);
    }

    PropertyType operator[](entity_system::entity_system::entity en) const {
        return m_container[lookup(en)];
    }

    typename ContainerType::reference operator[](entity_system::entity_system::entity en) {
        return m_container[lookup(en)];
    }

    typename ContainerType::const_reference at(entity_system::entity_system::entity en) const {
        return m_container.at(lookup(en));
    }

};

template <class OpaqueEntityType, class PropertyType>
class opaque_property : public property<PropertyType> {

    opaque_property(const entity_system & sys) :
        property<PropertyType>(sys)
    {

    }
public:

    class helper {
    public:
        static opaque_property<OpaqueEntityType, PropertyType> make_property(const entity_system & sys) {
            return std::move(opaque_property<OpaqueEntityType, PropertyType>(sys));
        }
    };

    friend class helper;

    PropertyType operator[](OpaqueEntityType en) const {
        return static_cast<property<PropertyType>*>(this)[en];
    }

    typename property<PropertyType>::ContainerType::reference operator[](OpaqueEntityType en) {
        return (*static_cast<property<PropertyType>*>(this))[en];
    }

    typename property<PropertyType>::ContainerType::const_reference at(OpaqueEntityType en) const {
        return this->at(en);
    }
};

template<class PropertyType>
property<PropertyType> make_property(const entity_system & sys) {
    return property<PropertyType>::helper::make_property(sys);
}

template <class OpaqueEntityType, class PropertyType>
opaque_property<OpaqueEntityType, PropertyType> make_opaque_property(const entity_system & sys) {
    return opaque_property<OpaqueEntityType, PropertyType>::helper::make_property(sys);
}


}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_PROPERTY_H
