#ifndef DRAWABLEBATCH_H
#define DRAWABLEBATCH_H

#include <SFML/Graphics.hpp>
#include <ophidian/geometry/Models.h>
#include <ophidian/circuit/Netlist.h>
#include <ophidian/entity_system/Property.h>

template<std::size_t NumberOfVertices>
class DrawableBatch : public sf::Drawable
{
public:
    DrawableBatch(sf::PrimitiveType primitive);
    DrawableBatch(ophidian::circuit::Netlist & netlist, sf::PrimitiveType primitive);
    ~DrawableBatch();

    const sf::Vertex operator[](const std::size_t i) const;
    void transform(ophidian::circuit::Cell & cell, const sf::Transform & trans);
    void paint(ophidian::circuit::Cell & cell, const sf::Color & color);
    void setPoint(ophidian::circuit::Cell & cell, std::size_t i, ophidian::geometry::Point &p);
    ophidian::geometry::Point point(ophidian::circuit::Cell & cell, std::size_t i) const;
    std::size_t vertexAmount() const;
    const std::array<sf::Vertex, NumberOfVertices> & points(ophidian::circuit::Cell & cell) const;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    // ??? bool has_animation() const;
    // ... void animate(batch_animation * animation);
    // ??? entity_system::entity create();
    // ??? void destroy(entity_system::entity the_entity);
    // ??? void update();
    // ??? void clear();

private:
    ophidian::entity_system::Property<ophidian::circuit::Cell, std::array<sf::Vertex, NumberOfVertices>> & mVertices;
    sf::PrimitiveType mPrimitive;
    // ... batch_animation * m_animation;
};

#endif // DRAWABLEBATCH_H
