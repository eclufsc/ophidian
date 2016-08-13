#ifndef OPHIDIAN_ENTITY_SYSTEM_ATTACHED_PROPERTY_H
#define OPHIDIAN_ENTITY_SYSTEM_ATTACHED_PROPERTY_H

#include <vector>
#include "entity_system.h"

namespace ophidian {
namespace entity_system {

template <class IteratorType>
class bounds {
    IteratorType m_begin;
    IteratorType m_end;
public:
    bounds(IteratorType begin, IteratorType end) :
        m_begin(begin),
        m_end(end)
    {
    }

    ~bounds() {

    }

    IteratorType begin() const {
        return m_begin;
    }

    IteratorType end() const {
        return m_end;
    }
};

class abstract_property {
public:
    virtual ~abstract_property() { }
    virtual void on_resize(std::size_t new_size) = 0;
    virtual void on_create(entity_system::entity en) = 0;
    virtual void on_destroy(entity_system::entity en) = 0;
    virtual void on_clear() = 0;
    virtual void system(const entity_system & system) = 0;
    virtual const entity_system& system() const = 0;
};

class attached_property : public abstract_property {
    const entity_system * m_system;
public:

    attached_property() :
        m_system(&entity_system::null()) {
        m_system->notifier()->attach(*this);
    }

    virtual ~attached_property() {
        if(m_system->notifier()->has_property(*this))
            m_system->notifier()->dettach(*this);
    }
    void system(const entity_system & system) {
        m_system->notifier()->dettach(*this);
        m_system = &system;
        m_system->notifier()->attach(*this);
    }

    const entity_system& system() const {
        return *m_system;
    }

};

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_ATTACHED_PROPERTY_H
