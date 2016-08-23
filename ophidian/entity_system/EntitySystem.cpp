#include "EntitySystem.h"

namespace ophidian {
namespace entity_system {

EntityBase::EntityBase() :
    id_(std::numeric_limits<uint32_t>::max()),
    system_(nullptr)
{

}

EntityBase::~EntityBase()
{

}

bool EntityBase::operator==(const EntityBase &entity) const
{
    return id_ == entity.id_ && system_ == entity.system_;
}

EntityBase::EntityBase(uint32_t id, EntitySystemBase * system) :
    id_(id),
    system_(system)
{

}

uint32_t EntitySystemBase::id(const EntityBase &en) const
{
    return en.id_;
}

}
}
