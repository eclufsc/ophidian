#ifndef ENTITYSYSTEM_H
#define ENTITYSYSTEM_H

#include <lemon/bits/alteration_notifier.h>
#include <lemon/maps.h>
#include <lemon/bits/vector_map.h>
#include <lemon/list_graph.h>
#include <iostream>
#include <vector>
#include <deque>

namespace ophidian
{
    namespace entity_system
    {

        class EntitySystemBase;
        class EntityBase
        {
            public:
                friend class EntitySystemBase;
                explicit EntityBase(uint32_t id, EntitySystemBase* system);
                EntityBase();
                ~EntityBase();
                bool operator==(const EntityBase& entity) const;
                bool operator!=(const EntityBase& entity) const;
            private:
                uint32_t id_;
                EntitySystemBase* system_;
        };

        class EntitySystemBase
        {
            public:
                uint32_t id(const EntityBase& en) const;
        };

        template <class Entity_>
        class EntitySystem : public EntitySystemBase
        {
            public:
                using Entity = Entity_;
                using NotifierType = lemon::AlterationNotifier<EntitySystem, Entity>;
                using ContainerType = std::vector<Entity>;
                using const_iterator = typename ContainerType::const_iterator;
                using size_type = typename ContainerType::size_type;

                EntitySystem()
                {
                    notifier_.setContainer(*this);
                    id_ = idCounter_++;
                }
                virtual ~EntitySystem()
                {

                }
                Entity add()
                {
                    uint32_t id = id2Index_.size();
                    Entity entity(id, this);
                    id2Index_.push_back(container_.size());
                    container_.push_back(entity);
                    notifier_.add(container_.back());
                    return entity;
                }
                void erase(const Entity& o)
                {
                    notifier_.erase(o);
                    auto index = id(o);
                    auto lastEntityId = id(container_.back());
                    std::swap(container_[index], container_.back());
                    container_.pop_back();
                    id2Index_[lastEntityId] = index;
                    id2Index_[index] = std::numeric_limits<uint32_t>::max();
                }
                void clear()
                {
                    notifier_.clear();
                    container_.clear();
                }
                void reserve(size_type size)
                {
                    container_.reserve(size);
                }

                size_type capacity() const
                {
                    return container_.capacity();
                }
                size_type size() const
                {
                    return container_.size();
                }
                bool empty() const
                {
                    return container_.empty();
                }
                const_iterator begin() const
                {
                    return container_.begin();
                }
                const_iterator end() const
                {
                    return container_.end();
                }
                bool valid(const Entity& o) const
                {
                    return EntitySystemBase::id(o) != std::numeric_limits<uint32_t>::max() &&
                           id2Index_[EntitySystemBase::id(o)] < container_.size();
                }
                NotifierType* notifier() const
                {
                    return const_cast<EntitySystem::NotifierType*>(&notifier_);
                }
                size_type id(const Entity& o) const
                {
                    return id2Index_.at(EntitySystemBase::id(o));
                }

                uint32_t id() const {
                    return id_;
                }

            private:
                NotifierType notifier_;
                ContainerType container_;
                std::vector<size_type> id2Index_;
                uint32_t id_;
                static uint32_t idCounter_;
        };

        template <class T>
        uint32_t EntitySystem<T>::idCounter_ = 0;


    }
}

#endif // ENTITYSYSTEM_H
