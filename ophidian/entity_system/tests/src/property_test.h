#ifndef PROPERTY_TEST_H
#define PROPERTY_TEST_H

#include <ophidian/entity_system/EntitySystem.h>

class MyEntity : public ophidian::entity_system::EntityBase {
public:
    MyEntity();
    MyEntity(uint32_t id, ophidian::entity_system::EntitySystemBase * system);
};

#endif // PROPERTY_TEST_H
