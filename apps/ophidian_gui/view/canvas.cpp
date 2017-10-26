#include "canvas.h"

Canvas::Canvas() :
    mQuads(sf::Quads, 4)
{

}

Canvas::~Canvas()
{

}

void Canvas::clear()
{
    mQuads.clear();
}

void Canvas::reserveMinimumOfQuads(std::size_t minimumOfQuads)
{
    mQuads.reserveMinimumOfQuads(minimumOfQuads);
}

DrawableBatch * Canvas::drawableQuads()
{
    return &mQuads;
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mQuads, states);
    //target.draw(test, states);
}

void Canvas::allocQuad(Quad & quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4)
{
    mQuads.alloc(quad, p1, p2, p3, p4);
}

void Canvas::quadUpdate(Quad & quad, const ophidian::geometry::Point & p1, const ophidian::geometry::Point & p2, const ophidian::geometry::Point & p3, const ophidian::geometry::Point & p4)
{

}
