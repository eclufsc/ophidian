#include "drawablebatch.h"

DrawableBatch::DrawableBatch(sf::PrimitiveType primitive, std::size_t numberOfVertices) :
    mNumberOfVertices(numberOfVertices),
    mPrimitive(primitive)
{
    std::array<sf::Vertex, 4> quad;
    quad[0].position = sf::Vector2f(10, 10);
    quad[1].position = sf::Vector2f(250, 10);
    quad[2].position = sf::Vector2f(250, 250);
    quad[3].position = sf::Vector2f(10, 250);
    mVertices.push_back(quad);

    std::cout << mVertices.size() << std::endl;

//    std::array<sf::Vertex, 4> quad2;
//    quad2[0].position = sf::Vector2f(1110, 1110);
//    quad2[1].position = sf::Vector2f(1350, 1110);
//    quad2[2].position = sf::Vector2f(1350, 1350);
//    quad2[3].position = sf::Vector2f(1110, 1350);
//    mVertices.push_back(quad2);

//    std::cout << mVertices.size() << std::endl;

}

DrawableBatch::~DrawableBatch()
{

}

void DrawableBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(mVertices.data()), mVertices.size()*mNumberOfVertices, mPrimitive, states);
}

const sf::Vertex DrawableBatch::operator[](const std::size_t i) const
{
    return reinterpret_cast<const sf::Vertex*>(mVertices.data())[i];
}

Quad DrawableBatch::create(const ophidian::geometry::Point p1, const ophidian::geometry::Point p2, const ophidian::geometry::Point p3, const ophidian::geometry::Point p4)
{
    Quad newQuad;
    std::array<sf::Vertex, 4> newVertices;

    newQuad.mId = mVertices.size();

    newVertices[0].position = sf::Vector2f(p1.x(), p1.y());
    newVertices[1].position = sf::Vector2f(p2.x(), p2.y());
    newVertices[2].position = sf::Vector2f(p3.x(), p3.y());
    newVertices[3].position = sf::Vector2f(p4.x(), p4.y());

    mVertices.push_back(newVertices);

    return newQuad;
}

void DrawableBatch::transform(const std::vector<Quad> &quads, const sf::Transform & trans)
{
    for (const Quad & q : quads)
    {
        auto box = mVertices[q.mId];
        for (sf::Vertex & v : box)
        {
            v.position = trans.transformPoint(v.position);
        }
    }
}


void DrawableBatch::paint(const std::vector<Quad> &quads, const sf::Color & color)
{
    for (const Quad & q : quads)
    {
        auto box = mVertices[q.mId];
        for (sf::Vertex & v : box)
        {
            v.color = color;
        }
    }
}


void DrawableBatch::setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p)
{
    mVertices[quad.mId][i].position = sf::Vector2f(p.x(), p.y());
}


ophidian::geometry::Point DrawableBatch::point(const Quad & quad, std::size_t i) const
{
    auto & verticesQuad = mVertices[quad.mId];
    return ophidian::geometry::Point(verticesQuad[i].position.x, verticesQuad[i].position.y);
}


std::size_t DrawableBatch::vertexAmount() const
{
    return mVertices.size() * mNumberOfVertices;
}


const std::array<sf::Vertex, 4> & DrawableBatch::points(const Quad & quad) const
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
