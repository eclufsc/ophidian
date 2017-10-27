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
    return &mQuads;
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mQuads, states);
    target.draw(mLines, states);
}
