#include "metal_layers.h"

namespace ophidian{
/// Namespace describing routing entities and basic routing interface.
namespace routing{

metal_layers::metal_layers()
{
    m_metal_layers_system.register_property(&m_routing_direction);
}

metal_layers::~metal_layers()
{

}

entity_system::entity metal_layers::layer_insert(ROUTING_DIRECTION direction, double pitch, double width)
{
    entity_system::entity the_layer = m_metal_layers_system.create();
    m_routing_direction[m_metal_layers_system.lookup(the_layer)] = direction;
    m_pitch[m_metal_layers_system.lookup(the_layer)] = pitch;
    m_width[m_metal_layers_system.lookup(the_layer)] = width;

    return the_layer;
}

}  /* namespace routing */
}  /* namespace ophidian */
