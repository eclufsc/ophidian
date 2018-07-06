#include "canvas.h"

Canvas::Canvas() :
    mQuads(sf::Quads),
    mLines(sf::Lines),
    mBoundaries(sf::Lines)
{

}

Canvas::~Canvas()
{

}

void Canvas::clear()
{
    mQuads.clear();
    mLines.clear();
    mBoundaries.clear();
}

void Canvas::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mQuads.reserveminimumOfForms(minimumOfQuads);
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mQuads, states);
    target.draw(mLines, states);
    target.draw(mBoundaries, states);
}

void Canvas::alloc(Quad & quad, const std::vector<ophidian::geometry::Point> & points)
{
    mQuads.alloc(quad, points);
}

void Canvas::desalloc(const std::vector<Quad> & quads)
{
    for (const auto & q : quads)
        mQuads.desalloc(q);
}

void Canvas::transform(const std::vector<Quad> & quads, const sf::Transform & trans)
{
    for (const auto & q : quads)
        mQuads.transform(q, trans);
}

void Canvas::paint(const std::vector<Quad> & quads, const sf::Color color)
{
    for (const auto & q : quads)
        mQuads.paint(q, color);
}

void Canvas::setPoint(const Quad & quad, std::size_t i, ophidian::geometry::Point & p)
{
    mQuads.setPoint(quad, i, p);
}

std::array<sf::Vertex, 4> Canvas::points(const Quad & quad)
{
    return mQuads.points(quad);
}

void Canvas::alloc(Line & line, const std::vector<ophidian::geometry::Point> & points)
{
    mLines.alloc(line, points);
}

void Canvas::desalloc(const std::vector<Line> & lines)
{
    for (const auto & l : lines)
        mLines.desalloc(l);
}

void Canvas::transform(const std::vector<Line> & lines, const sf::Transform & trans)
{
    for (const auto & l : lines)
        mLines.transform(l, trans);
}

void Canvas::paint(const std::vector<Line> & lines, const sf::Color color)
{
    for (const auto & l : lines)
        mLines.paint(l, color);
}

void Canvas::setPoint(const Line & line, std::size_t i, const ophidian::geometry::Point p)
{
    mLines.setPoint(line, i, p);
}

std::array<sf::Vertex, 2> Canvas::points(const Line & line)
{
    return mLines.points(line);
}

void Canvas::createBoundaries(const ophidian::geometry::Point chipUpperRightCorner)
{
    Line line;
    std::vector<ophidian::geometry::Point> points;

    ophidian::geometry::Point lowerLeft(0, 0);
    ophidian::geometry::Point lowerRight(chipUpperRightCorner.x(), 0);
    ophidian::geometry::Point upperRight(chipUpperRightCorner.x(), chipUpperRightCorner.y());
    ophidian::geometry::Point upperLeft(0, chipUpperRightCorner.y());

    points.push_back(lowerLeft);
    points.push_back(lowerRight);
    mBoundaries.alloc(line, points);
    points.clear();

    points.push_back(lowerRight);
    points.push_back(upperRight);
    mBoundaries.alloc(line, points);
    points.clear();

    points.push_back(upperRight);
    points.push_back(upperLeft);
    mBoundaries.alloc(line, points);
    points.clear();

    points.push_back(upperLeft);
    points.push_back(lowerLeft);
    mBoundaries.alloc(line, points);

}

WireQuad Canvas::createWireQuad(const ophidian::circuit::Cell & cell, const ophidian::geometry::Point & origin, const ophidian::geometry::Point & size)
{
    std::vector<ophidian::geometry::Point> points;
    std::array<Line, 4> lines;

    ophidian::geometry::Point lowerLeft (origin.x()         , origin.y()         );
    ophidian::geometry::Point lowerRight(origin.x()+size.x(), origin.y()         );
    ophidian::geometry::Point upperRight(origin.x()+size.x(), origin.y()+size.y());
    ophidian::geometry::Point upperLeft (origin.x()         , origin.y()+size.y());

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

    WireQuad wire(cell);
    wire.mLines = lines;
    return wire;
}

void Canvas::clear(WireQuad & wire)
{
    mLines.clear();
}
