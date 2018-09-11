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

void Canvas::alloc(Quad & quad, const std::vector<point_type> & points)
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

void Canvas::setPoint(const Quad & quad, std::size_t i, point_type & p)
{
    mQuads.setPoint(quad, i, p);
}

std::array<sf::Vertex, 4> Canvas::points(const Quad & quad)
{
    return mQuads.points(quad);
}

void Canvas::alloc(Line & line, const std::vector<point_type> & points)
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

void Canvas::setPoint(const Line & line, std::size_t i, const point_type p)
{
    mLines.setPoint(line, i, p);
}

std::array<sf::Vertex, 2> Canvas::points(const Line & line)
{
    return mLines.points(line);
}

void Canvas::createBoundaries(const point_type chipUpperRightCorner)
{
    Line line;
    std::vector<point_type> points;

    point_type lowerLeft(0, 0);
    point_type lowerRight(chipUpperRightCorner.x(), 0);
    point_type upperRight(chipUpperRightCorner.x(), chipUpperRightCorner.y());
    point_type upperLeft(0, chipUpperRightCorner.y());

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

WireQuad Canvas::createWireQuad(const ophidian::circuit::CellInstance & cell, const point_type & origin, const point_type & size)
{
    std::vector<point_type> points;
    std::array<Line, 4> lines;

    point_type lowerLeft (origin.x()         , origin.y()         );
    point_type lowerRight(origin.x()+size.x(), origin.y()         );
    point_type upperRight(origin.x()+size.x(), origin.y()+size.y());
    point_type upperLeft (origin.x()         , origin.y()+size.y());

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

    auto wire = WireQuad{cell};
    wire.mLines = lines;
    return wire;
}

void Canvas::clear(WireQuad & wire)
{
    mLines.clear();
}
