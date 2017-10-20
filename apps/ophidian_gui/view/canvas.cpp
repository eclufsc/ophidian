#include "canvas.h"

Canvas::Canvas()
{

}

Canvas::~Canvas()
{

}

void Canvas::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mQuads.reserveMinimumOfQuads(minimumOfQuads);
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mQuads, states);
    //target.draw(test, states);
}

Quad Canvas::quadCreate(const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4)
{
    return mQuads.create(p1, p2, p3, p4);
}

void Canvas::quadUpdate(Quad quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4)
{

}
