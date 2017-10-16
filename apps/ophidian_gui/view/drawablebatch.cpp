#include "drawablebatch.h"

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::DrawableBatch(sf::PrimitiveType primitive) :
    mPrimitive(primitive)
{

}

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::~DrawableBatch()
{

}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(mVertices), mVertices.size()*NumberOfVertices, mPrimitive, states);
}


template<std::size_t NumberOfVertices>
const sf::Vertex DrawableBatch<NumberOfVertices>::operator[](const std::size_t i) const
{
    return reinterpret_cast<const sf::Vertex*>(mVertices)[i];
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::transform(const std::vector<Quad> &quads, const sf::Transform & trans)
{
    for (Quad & q : quads)
    {
        auto box = mVertices[q.mId];
        for (sf::Vertex & v : box)
        {
            v.position = trans.transformPoint(v.position);
        }
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::paint(const std::vector<Quad> &quads, const sf::Color & color)
{
    for (Quad & q : quads)
    {
        auto box = mVertices[q.mId];
        for (sf::Vertex & v : box)
        {
            v.color = color;
        }
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p)
{
    mVertices[quad.mId][i].position = sf::Vector2f(p.x(), p.y());
}

template<std::size_t NumberOfVertices>
ophidian::geometry::Point DrawableBatch<NumberOfVertices>::point(const Quad & quad, std::size_t i) const
{
    auto & verticesQuad = mVertices[quad.mId];
    return ophidian::geometry::Point(verticesQuad[i].position.x, verticesQuad[i].position.y);
}

template<std::size_t NumberOfVertices>
std::size_t DrawableBatch<NumberOfVertices>::vertexAmount() const
{
    return mVertices.size() * NumberOfVertices;
}

template<std::size_t NumberOfVertices>
const std::array<sf::Vertex, NumberOfVertices> & DrawableBatch<NumberOfVertices>::points(const Quad & quad) const
{
    return mVertices[quad.mId];
}

/*

DrawableBatch::DrawableBatch(sf::PrimitiveType primitive) :
    m_primitive(primitive),
    m_animation(nullptr){
    m_system.register_property(&m_vertices);
    m_system.preallocate(10000000);
}

bool DrawableBatch::has_animation() const {
    return m_animation != nullptr;
}

const sf::Vertex operator[](const std::size_t i) const {
    return reinterpret_cast<const sf::Vertex*>(m_vertices.data())[i];
}


void DrawableBatch::update()
{
    if(m_animation)
        m_animation = m_animation->update(reinterpret_cast<sf::Vertex*>(m_vertices.data()));
}


void DrawableBatch::clear() {
    for(auto entity : m_system)
        m_system.destroy(entity);
}

*/
