#ifndef ENTITYSYSTEMMAP_H
#define ENTITYSYSTEMMAP_H

#include <lemon/maps.h>
#include "EntitySystem.h"

namespace ophidian
{
    namespace entity_system
    {
        template <class Entity_, class Value_>
        class Property :
            public lemon::MapBase<Entity_, Value_>,
            public EntitySystem<Entity_>::NotifierType::ObserverBase
        {
            public:
                using Parent = typename EntitySystem<Entity_>::NotifierType::ObserverBase;
                using Value = Value_;
                using Entity = Entity_;
                using ContainerType = std::vector<Value>;

                Property(const EntitySystem<Entity_>& system, Value defaultValue = Value()) :
                    Parent(*system.notifier()),
                    defaultValue_(defaultValue)
                {
                    properties_.reserve(system.capacity());
                    properties_.resize(system.size());
                    properties_.assign(properties_.size(), defaultValue_);
                }

                Property() :
                    Parent()
                {

                }

                Property& operator=(const Property& o)
                {
                    properties_ = o.properties_;
                    Parent::attach(*o.notifier());
                    return *this;
                }

                ~Property() override
                {

                }

                typename ContainerType::reference operator[](const Entity& entity)
                {
                    return properties_[Parent::notifier()->id(entity)];
                }
                typename ContainerType::const_reference operator[](const Entity& entity) const
                {
                    return properties_[Parent::notifier()->id(entity)];
                }

                typename ContainerType::iterator begin()
                {
                    return properties_.begin();
                }
                typename ContainerType::iterator end()
                {
                    return properties_.end();
                }

                typename ContainerType::const_iterator begin() const
                {
                    return properties_.begin();
                }
                typename ContainerType::const_iterator end() const
                {
                    return properties_.end();
                }
                typename ContainerType::size_type size() const
                {
                    return properties_.size();
                }
                bool empty() const
                {
                    return properties_.empty();
                }

                void reserve(std::uint32_t size) override
                {
                    properties_.reserve(size);
                }

                void shrink() override
                {
                    properties_.shrink_to_fit();
                }

                std::uint32_t capacity() const
                {
                    return properties_.capacity();
                }

            protected:

                virtual void add(const Entity& item) override
                {
                    properties_.push_back(defaultValue_);
                }
                virtual void add(const std::vector<Entity>& items) override
                {
                    properties_.resize(properties_.size() + items.size(), defaultValue_);
                }
                virtual void erase(const Entity& item) override
                {
                    std::swap(properties_.back(), properties_[Parent::notifier()->id(item)]);
                    properties_.pop_back();
                }

                virtual void clear() override
                {
                    properties_.clear();
                }

            protected:
                ContainerType properties_;
            private:
                const Value defaultValue_;
        };
    }
}

#endif // ENTITYSYSTEMMAP_H
