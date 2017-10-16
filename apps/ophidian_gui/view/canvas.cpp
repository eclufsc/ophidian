#include "canvas.h"

Canvas::Canvas()
{

}

Canvas::~Canvas()
{

}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(mQuads, states);
    target.draw(mQuads, states);
}
