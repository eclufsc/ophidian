#include <ophidian/entity_system/EntitySystem.h>
#include <ophidian/entity_system/Property.h>
#include <array>
#include <cassert>
#include <iostream>

using namespace ophidian;
template<class K, class V>
using Property = entity_system::Property<K,V>;

struct LUTIndex : public entity_system::EntityBase {
    using EntityBase::EntityBase;
};

template <class Entity_, class InputValue, class OutputValue, class InputCollection>
void findIndices(const entity_system::EntitySystem<Entity_> & sys, const Property<Entity_, InputValue>& inputReferences, const Property<Entity_, InputCollection>& input, Property<Entity_, OutputValue> & output) 
{
    auto enIt = sys.begin();
    auto refIt = inputReferences.begin();
    auto inIt = input.begin();
    auto outIt = output.begin();
    
    for(; enIt != sys.end() && refIt != inputReferences.end() && inIt != input.end() && outIt != output.end(); ++enIt, ++refIt, ++inIt, ++outIt) {
        (*outIt) = std::distance(inIt->begin(), std::lower_bound(inIt->begin(), inIt->end(), *refIt));
    }
}

int main(int argc, char *argv[])
{

    entity_system::EntitySystem<LUTIndex> sys;
    auto references = entity_system::Property<LUTIndex, double>(sys);
    auto values = entity_system::Property<LUTIndex, std::array<double, 3>>(sys);
    auto indices = entity_system::Property<LUTIndex, std::array<double, 3>::size_type>(sys);

    sys.add();
    sys.add();

    references[*sys.begin()] = 0.3;
    references[*std::next(sys.begin())] = 4.9;

    values[*sys.begin()] = {0.0, 1.0, 2.0};
    values[*std::next(sys.begin())] = {5.0, 6.0, 7.0};

    findIndices(sys, references, values, indices);

    std::cout << "It's OK " << indices[*sys.begin()] << " " << indices[*std::next(sys.begin())] << std::endl;
    assert(indices[*sys.begin()] == 1);
    assert(indices[*std::next(sys.begin())] == 0);

    return 0;
}
