#ifndef COMPOSITION_TEST_H
#define COMPOSITION_TEST_H

#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Composition.h>


class PartEntity : public ophidian::entity_system::EntityBase {
public:
    PartEntity(uint32_t id, ophidian::entity_system::EntitySystemBase * system);
    PartEntity();
};

class WholeEntity : public ophidian::entity_system::EntityBase {
public:
    WholeEntity(uint32_t id, ophidian::entity_system::EntitySystemBase * system);
    WholeEntity();
};

using WholeSystem = ophidian::entity_system::EntitySystem<WholeEntity> ;
using PartSystem = ophidian::entity_system::EntitySystem<PartEntity> ;
using SimpleComposition = ophidian::entity_system::Composition<WholeSystem::Entity, PartSystem::Entity>;

namespace fixture {

class PartAndWholeSystem {
public:
    PartAndWholeSystem();
    WholeSystem & wholes();
    PartSystem & parts();
    SimpleComposition & composition();
private:
    WholeSystem whole_;
    PartSystem part_;
    SimpleComposition composition_;
};

template <class T>
class AddPart : public T {
public:
    AddPart() :
        T(),
        p_(T::parts().add())
    {
    }
    const PartEntity & part() const {
        return p_;
    }
private:
    PartEntity p_;
};

template <class T>
class AddWhole : public T {
public:
    AddWhole() :
        T(),
        w_(T::wholes().add())
    {

    }
    const WholeEntity & whole() const {
        return w_;
    }
private:
    WholeEntity w_;
};

template <class T>
class AddPartToWhole : public T {
public:
    AddPartToWhole() :
        T()
    {
        T::composition().addAssociation(T::whole(), T::part());
    }
};


}

#endif // COMPOSITION_TEST_H
