#ifndef DRAWABLEBATCH_H
#define DRAWABLEBATCH_H

#include <iostream>

#include <vector>
#include <SFML/Graphics.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/circuit/Netlist.h>

#include "./Model/forms.h"

template<std::size_t NumberOfVertices>
class DrawableBatch : public sf::Drawable
{
public:
    DrawableBatch(sf::PrimitiveType primitive);
    ~DrawableBatch();

    void clear();

    const sf::Vertex operator[](const std::size_t i) const;
    void reserveMinimumOfQuads(std::size_t minimumOfQuads);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void alloc(Form & form, const std::vector<ophidian::geometry::Point> & points);
    void transform(const std::vector<Form> & forms, const sf::Transform & trans);
    void paint(const std::vector<Form> & forms, const sf::Color color);
    void setPoint(const Form & form, std::size_t i, const ophidian::geometry::Point & p);

    std::size_t vertexAmount() const;

    ophidian::geometry::Point point(const Form & form, std::size_t i) const;
    std::array<sf::Vertex, NumberOfVertices> points(const Form & form) const;

    // ??? bool has_animation() const;
    // ... void animate(batch_animation * animation);
    // ??? entity_system::entity create();
    // ??? void destroy(entity_system::entity the_entity);
    // ??? void update();


private:
    sf::PrimitiveType mPrimitive;
    std::vector<std::array<sf::Vertex, NumberOfVertices>> mVertices;
    // ... batch_animation * m_animation;
};

/* Implementation in .h because of the template. */

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::DrawableBatch(sf::PrimitiveType primitive) :
    mPrimitive(primitive)
{
    std::array<sf::Vertex, NumberOfVertices> form;

    if (mPrimitive == sf::Quads)
    {
        form[0].position = sf::Vector2f(10, 10);
        form[1].position = sf::Vector2f(250, 10);
        form[2].position = sf::Vector2f(250, 250);
        form[3].position = sf::Vector2f(10, 250);
    }
    else
    {
        form[0].position = sf::Vector2f(10, 500);
        form[1].position = sf::Vector2f(500, 500);
    }

    mVertices.push_back(form);
}

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::~DrawableBatch()
{

}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::clear()
{
    mVertices.clear();
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mVertices.reserve(minimumOfQuads);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::draw(sf::RenderTarget & target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(mVertices.data()), mVertices.size() * NumberOfVertices, mPrimitive, states);
}

template<std::size_t NumberOfVertices>
const sf::Vertex DrawableBatch<NumberOfVertices>::operator[](const std::size_t i) const
{
    return reinterpret_cast<const sf::Vertex*>(mVertices.data())[i];
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::alloc(Form & form, const std::vector<ophidian::geometry::Point> & points)
{
    form.mId = mVertices.size();

    std::array<sf::Vertex, NumberOfVertices> newVertices;

    for (auto i = 0; i < NumberOfVertices; ++i)
    {
        newVertices[i].position = sf::Vector2f(points[i].x(), points[i].y());
    }

    /*
    sf::Color color;
    switch (quad.mId % NumberOfVertices) {
    case 0:
        color = sf::Color::Red;
        break;
    case 1:
        color = sf::Color::Yellow;
        break;
    case 2:
        color = sf::Color::Green;
        break;
    case 3:
        color = sf::Color::Blue;
        break;
    default:
        break;
    }

    newVertices[0].color = color;
    newVertices[1].color = color;
    newVertices[2].color = color;
    newVertices[3].color = color;
    */

    mVertices.push_back(newVertices);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::transform(const std::vector<Form> &forms, const sf::Transform & trans)
{
    for (const Form & q : forms)
    {
        for (sf::Vertex & v : mVertices[q.mId])
        {
            v.position = trans.transformPoint(v.position);
        }
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::paint(const std::vector<Form> & forms, const sf::Color color)
{
    for (const Form & q : forms)
    {
        std::array<sf::Vertex, NumberOfVertices> & box = mVertices[q.mId];
        for (sf::Vertex & v : box)
        {
            v.color = color;
        }
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::setPoint(const Form & form, std::size_t i, const ophidian::geometry::Point & p)
{
    mVertices[form.mId][i].position = sf::Vector2f(p.x(), p.y());
}

template<std::size_t NumberOfVertices>
ophidian::geometry::Point DrawableBatch<NumberOfVertices>::point(const Form & form, std::size_t i) const
{
    auto & verticesQuad = mVertices[form.mId];
    return ophidian::geometry::Point(verticesQuad[i].position.x, verticesQuad[i].position.y);
}

template<std::size_t NumberOfVertices>
std::size_t DrawableBatch<NumberOfVertices>::vertexAmount() const
{
    return mVertices.size() * NumberOfVertices;
}

template<std::size_t NumberOfVertices>
std::array<sf::Vertex, NumberOfVertices> DrawableBatch<NumberOfVertices>::points(const Form & form) const
{
    return mVertices[form.mId];
}

/*

DrawableBatch<NumberOfVertices>::DrawableBatch(sf::PrimitiveType primitive) :
    m_primitive(primitive),
    m_animation(nullptr){
    m_system.register_property(&m_vertices);
    m_system.preallocate(10000000);
}

bool DrawableBatch<NumberOfVertices>::has_animation() const {
    return m_animation != nullptr;
}

void DrawableBatch<NumberOfVertices>::update()
{
    if(m_animation)
        m_animation = m_animation->update(reinterpret_cast<sf::Vertex*>(m_vertices.data()));
}


void DrawableBatch<NumberOfVertices>::clear() {
    for(auto entity : m_system)
        m_system.destroy(entity);
}

*/

#endif // DRAWABLEBATCH_H
