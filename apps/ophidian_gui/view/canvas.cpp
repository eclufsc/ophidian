#include "canvas.h"

Canvas::Canvas() :
    mQuads(sf::Quads),
    mLines(sf::Lines)
{

}

Canvas::~Canvas()
{

}

void Canvas::clear()
{
    mQuads.clear();
    mLines.clear();
}

void Canvas::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mQuads.reserveMinimumOfQuads(minimumOfQuads);
}

DrawableBatch<4> * Canvas::drawableQuads()
{
    return & mQuads;
}

DrawableBatch<2> * Canvas::drawableLines()
{
    return & mLines;
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mQuads, states);
    target.draw(mLines, states);
}

void Canvas::alloc(Quad & quad, const std::vector<ophidian::geometry::Point> & points)
{
    mQuads.alloc(quad, points);
}

void Canvas::transform(Quad, const std::vector<Form> & quads, const sf::Transform & trans)
{
    mQuads.transform(quads, trans);
}

void Canvas::paint(Quad, const std::vector<Form> & quads, const sf::Color color)
{
    mQuads.paint(quads, color);
}

void Canvas::setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p)
{
    mQuads.setPoint(quad, i, p);
}

void Canvas::alloc(Line & line, const std::vector<ophidian::geometry::Point> & points)
{
    mLines.alloc(line, points);
}

void Canvas::transform(Line, const std::vector<Form> & lines, const sf::Transform & trans)
{
    mLines.transform(lines, trans);
}

void Canvas::paint(Line, const std::vector<Form> & lines, const sf::Color color)
{
    mLines.paint(lines, color);
}

void Canvas::setPoint(const Line & line, std::size_t i, ophidian::geometry::Point & p)
{
    mLines.setPoint(line, i, p);
}

WireQuad Canvas::createWireQuad(const std::vector<Quad> & quads)
{
    std::vector<ophidian::geometry::Point> points;
    std::vector<std::array<sf::Vertex, 4>> pointsOfQuads;
    std::array<Line, 4> lines;
    for (const auto & q : quads)
    {
        pointsOfQuads.push_back(mQuads.points(q));
    }

    ophidian::geometry::Point lowerLeft(pointsOfQuads.begin()->at(0).position.x, pointsOfQuads.begin()->at(0).position.y);
    ophidian::geometry::Point lowerRight(pointsOfQuads.begin()->at(1).position.x, pointsOfQuads.begin()->at(1).position.y);
    ophidian::geometry::Point upperRight(pointsOfQuads.back().at(2).position.x, pointsOfQuads.back().at(2).position.y);
    ophidian::geometry::Point upperLeft(pointsOfQuads.back().at(3).position.x, pointsOfQuads.back().at(3).position.y);

    lines[0] = Line();
    points.push_back(lowerLeft);
    points.push_back(lowerRight);
    mLines.alloc(lines[0], points);
    points.clear();

    lines[1] = Line();
    points.push_back(lowerRight);
    points.push_back(upperRight);
    mLines.alloc(lines[1], points);
    points.clear();

    lines[2] = Line();
    points.push_back(upperRight);
    points.push_back(upperLeft);
    mLines.alloc(lines[2], points);
    points.clear();

    lines[3] = Line();
    points.push_back(upperLeft);
    points.push_back(lowerLeft);
    mLines.alloc(lines[3], points);

    WireQuad wire(quads.begin()->mCell);
    wire.mLines = lines;
    return wire;
}

void Canvas::clear(WireQuad & wire)
{
    mLines.clear();
}
