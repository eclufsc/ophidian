#include "entity_system.h"
#include "property.h"

#include <limits>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

namespace ophidian {
namespace entity_system {


using entity = entity_system::entity;

using StorageType = std::vector<entity>;

// entity_system::iterator PIMPL
struct entity_system::const_iterator::impl {
    StorageType::const_iterator it;
};

entity_system::const_iterator::const_iterator() :
    this_(new impl)
{

}

entity_system::const_iterator::~const_iterator()
{

}

entity_system::const_iterator::const_iterator(const entity_system::const_iterator &o) :
    const_iterator()
{
    (*this) = o;
}

entity_system::const_iterator &entity_system::const_iterator::operator=(const entity_system::const_iterator &o)
{
    this_->it = o.this_->it;
    return *this;
}

const entity &entity_system::const_iterator::operator*() const
{
    return *this_->it;
}

const entity_system::const_iterator &entity_system::const_iterator::operator++(void)
{
    ++this_->it;
    return *this;
}

entity_system::const_iterator::difference_type entity_system::const_iterator::operator-(const entity_system::const_iterator &o) const
{
    return this_->it - o.this_->it;
}

bool entity_system::const_iterator::operator==(const entity_system::const_iterator &it) const
{
    return this_->it == it.this_->it;
}

bool entity_system::const_iterator::operator!=(const entity_system::const_iterator &it) const
{
    return this_->it != it.this_->it;
}

entity_system::const_iterator::const_iterator(const impl &i) :
    this_(new impl(i))
{

}
// entity_system PIMPL

class entity_system::impl {
public:
    impl() :
        m_notifier(new notifier_)
    {

    }

    inline  std::size_t size() const {
        return m_entities.size();
    }

    inline std::size_t lookup(entity e) const {
        std::size_t value = m_id2index.at(e.id());
        if(value == entity::invalid())
            throw std::runtime_error("Lookup on a dead entity (id = " + std::to_string(e.id()) + ")");
        return value;
    }

    inline entity create()
    {
        entity new_entity(create_new_id());
        m_entities.push_back(new_entity);
        m_notifier->create(new_entity);
        return new_entity;
    }

    inline void destroy(entity e) {
        destroy_properties(e);
        swap_current_and_last_entity(e);
        remove_last_entity(e);
    }

    inline bool valid(entity e) const
    {
        return (e != entity::null()) && (m_id2index.at(e.id()) != entity::invalid());
    }

    inline void clear()
    {
        m_notifier->clear();
        m_entities.clear();
        std::fill(m_id2index.begin(), m_id2index.end(), entity::invalid());
    }

    inline notifier_ *notifier() const {
        return const_cast<notifier_*>(m_notifier.get());
    }

    inline bool has_property(abstract_property &prop) const {
        return m_notifier->has_property(prop);
    }

    inline bool operator==(const impl& other) const {
        return this == &other;
    }

    inline const const_iterator begin() const {
        return const_iterator::impl{m_entities.begin()};
    }

    inline const const_iterator end() const {
        return const_iterator::impl{m_entities.end()};
    }

    inline std::size_t properties_size() const {
        return m_notifier->properties_size();
    }

private:
    inline std::size_t create_new_id()
    {
        m_id2index.push_back(m_entities.size());
        return (m_id2index.size()-1);
    }

    inline void swap_current_and_last_entity(entity e)
    {
        std::size_t index = m_id2index.at(e.id());
        m_id2index.at(m_entities.back().id()) = index;
        std::swap(m_entities.at(index), m_entities.back());
    }

    inline void remove_last_entity(entity e)
    {
        m_entities.pop_back();
        m_id2index.at(e.id()) = entity::invalid();
    }

    inline void destroy_properties(entity e)
    {
        m_notifier->destroy(e);
    }

    std::vector<std::size_t> m_id2index;
    StorageType m_entities;
    std::unique_ptr<notifier_> m_notifier;
};

entity_system::entity_system() :
    this_(new impl)
{

}

entity_system::~entity_system()
{

}

std::size_t entity_system::size() const {
    return this_->size();
}

std::size_t entity_system::lookup(entity_system::entity e) const {
    return this_->lookup(e);
}

entity entity_system::create()
{
    return this_->create();
}

void entity_system::destroy(entity e)
{
    this_->destroy(e);
}

bool entity_system::valid(entity e) const
{
    return this_->valid(e);
}

void entity_system::clear()
{
    this_->clear();
}

entity_system::notifier_ *entity_system::notifier() const {
    return this_->notifier();
}

bool entity_system::has_property(abstract_property &prop) const {
    return this_->has_property(prop);
}

bool entity_system::operator==(const entity_system &other) const {
    return (*this_) == (*other.this_);
}

const entity_system::const_iterator entity_system::begin() const {
    return this_->begin();
}

const entity_system::const_iterator entity_system::end() const {
    return this_->end();
}

const entity_system &entity_system::null() {
    static entity_system es;
    return es;
}

std::size_t entity_system::properties_size() const {
    return this_->properties_size();
}

entity::entity() : m_id(invalid())
{

}

entity::entity(const entity &o)
{
    *this = o;
}

entity &entity::operator=(const entity &o)
{
    m_id = o.m_id;
    return *this;
}

bool entity::operator<(const entity &o) const
{
    return m_id < o.m_id;
}

bool entity::operator==(const entity &o) const
{
    return m_id == o.m_id;
}

bool entity::operator!=(const entity &o) const
{
    return !((*this) == o);
}

entity_system::entity entity::null()
{
    static entity null_entity;
    return null_entity;
}

std::size_t entity::invalid()
{
    return std::numeric_limits<std::size_t>::max();
}

// entity_system::notifier_ PIMPL
struct entity_system::notifier_::impl {

    impl() {
        m_properties.reserve(64);
    }

    ~impl() {
    }

    inline void attach(abstract_property &prop) {
        m_properties.push_back(&prop);
    }

    inline void dettach(abstract_property &prop)
    {
        if(!has_property(prop))
            throw std::runtime_error("can't dettach unattached property");
        m_properties.erase(std::find(m_properties.begin(), m_properties.end(), &prop));
    }

    inline void create(entity_system::entity en) {
        for(auto prop : m_properties)
            prop->on_create(en);
    }

    inline void destroy(entity_system::entity en) {
        // workaround to fix self association issues
        std::for_each(m_properties.crbegin(), m_properties.crend(), [en](abstract_property * prop){
            prop->on_destroy(en);
        });
        /*for(auto prop : m_properties)
            prop->on_destroy(en);*/
    }

    inline void clear() {
        for(auto prop : m_properties)
            prop->on_clear();
    }

    inline bool has_property(abstract_property &prop) const {
        return (std::count(m_properties.begin(), m_properties.end(), &prop) == 1);
    }

    inline std::size_t properties_size() const
    {
        return m_properties.size();
    }

private:
    std::vector<abstract_property*> m_properties;

};

// entity_system::notifier_
entity_system::notifier_::notifier_() :
    this_(new impl)
{

}

entity_system::notifier_::~notifier_()
{
}

void entity_system::notifier_::attach(abstract_property &prop)
{
    this_->attach(prop);
}

void entity_system::notifier_::dettach(abstract_property &prop)
{
    this_->dettach(prop);
}

void entity_system::notifier_::create(entity en)
{
    this_->create(en);
}

void entity_system::notifier_::destroy(entity en)
{
    this_->destroy(en);
}

void entity_system::notifier_::clear()
{
    this_->clear();
}

std::size_t entity_system::notifier_::properties_size() const
{
    return this_->properties_size();
}


bool entity_system::notifier_::has_property(abstract_property &prop) const
{
    return this_->has_property(prop);
}

}
}
