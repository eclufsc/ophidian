#ifndef DRAWABLEBATCH_H
#define DRAWABLEBATCH_H

#include <SFML/Graphics.hpp>

#include <vector>

#include "./Model/forms.h"

#include <ophidian/geometry/Models.h>

template<std::size_t NumberOfVertices>
class DrawableBatch : public sf::Drawable
{
public:
    DrawableBatch(sf::PrimitiveType primitive);
    ~DrawableBatch();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    const sf::Vertex operator[](const std::size_t i) const;

    void transform(const std::vector<Quad> & quads, const sf::Transform & trans);
    void paint(const std::vector<Quad> & quads, const sf::Color & color);
    void setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p);

    std::size_t vertexAmount() const;

    ophidian::geometry::Point point(const Quad & quad, std::size_t i) const;
    const std::array<sf::Vertex, NumberOfVertices> & points(const Quad & quad) const;

    // ??? bool has_animation() const;
    // ... void animate(batch_animation * animation);
    // ??? entity_system::entity create();
    // ??? void destroy(entity_system::entity the_entity);
    // ??? void update();
    // ??? void clear();

private:
    std::vector<std::array<sf::Vertex, NumberOfVertices>> mVertices;
    sf::PrimitiveType mPrimitive;
    // ... batch_animation * m_animation;
};

#endif // DRAWABLEBATCH_H
