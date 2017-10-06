#include "drawablebatch.h"

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::DrawableBatch(ophidian::circuit::Netlist & netlist, sf::PrimitiveType primitive) :
    mVertices(netlist.makeProperty<std::array<sf::Vertex, NumberOfVertices>>(ophidian::circuit::Cell())),
    mPrimitive(primitive)
{

}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::transform(ophidian::circuit::Cell & cell, const sf::Transform & trans)
{
    auto & cellVertices = mVertices[cell];
    for (sf::Vertex & v : cellVertices)
    {
        v.position = trans.transformPoint(v.position);
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::paint(ophidian::circuit::Cell & cell, const sf::Color & color)
{
    auto & cellVertices = mVertices[cell];
    for (sf::Vertex & v : cellVertices)
    {
        v.color = color;
    }
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::setPoint(ophidian::circuit::Cell & cell, std::size_t i, ophidian::geometry::Point &p)
{
    mVertices[cell][i].position = sf::Vector2f(p.x(), p.y());
}

template<std::size_t NumberOfVertices>
ophidian::geometry::Point DrawableBatch<NumberOfVertices>::point(ophidian::circuit::Cell & cell, std::size_t i) const
{
    auto & verticesCell = mVertices[cell];
    return ophidian::geometry::Point(verticesCell[i].position.x, verticesCell[i].position.y);
}

template<std::size_t NumberOfVertices>
std::size_t DrawableBatch<NumberOfVertices>::vertexAmount() const
{
    return mVertices.size() * NumberOfVertices;
}

template<std::size_t NumberOfVertices>
const std::array<sf::Vertex, NumberOfVertices> & DrawableBatch<NumberOfVertices>::points(ophidian::circuit::Cell & cell) const
{
    return mVertices[cell];
}

/*DrawableBatch::DrawableBatch(sf::PrimitiveType primitive) :
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

void DrawableBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(m_vertices.data()), m_system.size()*NumberOfVertices, m_primitive, states);
}

void DrawableBatch::clear() {
    for(auto entity : m_system)
        m_system.destroy(entity);
}

*/
