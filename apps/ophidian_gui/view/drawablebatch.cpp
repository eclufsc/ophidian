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
}

DrawableBatch::~DrawableBatch()
{

}

void DrawableBatch::clear()
{
    mVertices.clear();
}

void DrawableBatch::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mVertices.reserve(minimumOfQuads);
}

void DrawableBatch::draw(sf::RenderTarget & target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(mVertices.data()), mVertices.size() * mNumberOfVertices, mPrimitive, states);
}

const sf::Vertex DrawableBatch::operator[](const std::size_t i) const
{
    return reinterpret_cast<const sf::Vertex*>(mVertices.data())[i];
}

void DrawableBatch::alloc(Quad & quad, const std::vector<ophidian::geometry::Point> & points)
{
    quad.mId = mVertices.size();

    std::array<sf::Vertex, 4> newVertices;

    for (auto i = 0; i < 4; ++i)
    {
        newVertices[i].position = sf::Vector2f(points[i].x(), points[i].y());
    }

    /*
    sf::Color color;
    switch (quad.mId % 4) {
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


void DrawableBatch::paint(const std::vector<Quad> & quads, const sf::Color color)
{
    for (const Quad & q : quads)
    {
        std::array<sf::Vertex, 4> & box = mVertices[q.mId];
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
