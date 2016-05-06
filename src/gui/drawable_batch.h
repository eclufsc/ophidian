#ifndef OPHIDIAN_GUI_DRAWABLE_BATCH_H
#define OPHIDIAN_GUI_DRAWABLE_BATCH_H

#include <SFML/Graphics.hpp>
#include "../geometry/geometry.h"
#include "../entity/entity.h"
#include "../entity/vector_property.h"

namespace ophidian {
namespace gui {

class batch_animation {
    std::vector< sf::Vertex > m_delta;
    const std::size_t c_DURATION;
    std::size_t m_current;
public:
    template <class T>
    batch_animation(T & batch, std::size_t duration) :
        m_delta(batch.vertex_count()),
        c_DURATION(duration),
        m_current(0)
    {
    }

    sf::Vertex& operator[](const std::size_t i)
    {
        return m_delta[i];
    }

    batch_animation* update(sf::Vertex * v) {
        std::cout << m_current << " / " << c_DURATION << std::endl;
        if(m_current == c_DURATION)
        {
            delete this;
            return nullptr;
        }
        for(std::size_t i = 0; i < m_delta.size(); ++i)
        {
            v[i].position.x += m_delta[i].position.x/static_cast<double>(c_DURATION);
            v[i].position.y += m_delta[i].position.y/static_cast<double>(c_DURATION);
        }
        ++m_current;
        return this;
    }
};


template<std::size_t NumberOfVertices>
class drawable_batch : public sf::Drawable
{
    entity::system m_system;
    entity::vector_property< std::array<sf::Vertex, NumberOfVertices> > m_vertices;
    sf::PrimitiveType m_primitive;

    batch_animation * m_animation;

public:
    drawable_batch(sf::PrimitiveType primitive) :
        m_primitive(primitive),
        m_animation(nullptr){
        m_system.register_property(&m_vertices);
        m_vertices.preallocate(10000000);
    }

    const sf::Vertex operator[](const std::size_t i) const {
        return reinterpret_cast<const sf::Vertex*>(m_vertices.data())[i];
    }

    void animate(batch_animation * animation)
    {
        m_animation = animation;
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

    std::size_t vertex_count() const {
        return m_system.size()*NumberOfVertices;
    }


    void update()
    {
        if(m_animation)
            m_animation = m_animation->update(reinterpret_cast<sf::Vertex*>(m_vertices.data()));
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(reinterpret_cast<const sf::Vertex*>(m_vertices.data()), m_system.size()*NumberOfVertices, m_primitive, states);
    }

};



}
}

#endif // OPHIDIAN_GUI_DRAWABLE_BATCH_H
