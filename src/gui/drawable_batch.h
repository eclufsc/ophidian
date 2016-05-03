#ifndef OPHIDIAN_GUI_DRAWABLE_BATCH_H
#define OPHIDIAN_GUI_DRAWABLE_BATCH_H

#include <SFML/Graphics.hpp>
#include "../geometry/geometry.h"
#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace gui {

template<std::size_t NumberOfVertices>
class drawable_batch : public sf::Drawable
{
    entity::system m_system;
    entity::vector_property< std::array<sf::Vertex, NumberOfVertices> > m_vertices;
    sf::PrimitiveType m_primitive;
public:
    drawable_batch(sf::PrimitiveType primitive) :
        m_primitive(primitive){
        m_system.register_property(&m_vertices);
    }

    entity::entity create() {
        auto the_entity = m_system.create();
        paint(the_entity, sf::Color::Black);
        return the_entity;
    }

    void destroy(entity::entity the_entity) {
        m_system.destroy(the_entity);
    }

    void transform(entity::entity the_entity, const sf::Transform & trans)
    {
        std::array<sf::Vertex, NumberOfVertices> & vertices = m_vertices[m_system.lookup(the_entity)];
        for(sf::Vertex & v : vertices)
            v.position = trans.transformPoint(v.position);
    }

    void paint(entity::entity the_entity, const sf::Color & color)
    {
        std::array<sf::Vertex, NumberOfVertices> & vertices = m_vertices[m_system.lookup(the_entity)];
        for(sf::Vertex & v : vertices)
            v.color = color;
    }

    void set_point(entity::entity the_entity, std::size_t i, const geometry::point<double> &p)
    {
        std::array<sf::Vertex, NumberOfVertices> & vertices = m_vertices[m_system.lookup(the_entity)];
        vertices.at(i).position = sf::Vector2f(p.x(), p.y());
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(reinterpret_cast<const sf::Vertex*>(m_vertices.data()), m_system.size()*NumberOfVertices, m_primitive, states);
    }

};

}
}

#endif // OPHIDIAN_GUI_DRAWABLE_BATCH_H
