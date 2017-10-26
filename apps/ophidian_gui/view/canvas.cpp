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
