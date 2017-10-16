#include "drawpieces.h"

DrawPieces::DrawPieces(sf::PrimitiveType primitive) :
    mPrimitive(primitive)
{

}

DrawPieces::~DrawPieces()
{

}

void DrawPieces::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::VertexArray quad(sf::Quads, 4);
    quad[0].position = sf::Vector2f(10, 10);
    quad[1].position = sf::Vector2f(250, 10);
    quad[2].position = sf::Vector2f(250, 250);
    quad[3].position = sf::Vector2f(10, 250);
    target.draw(quad, states);
}
