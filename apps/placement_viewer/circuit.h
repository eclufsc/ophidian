#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <SFML/Graphics.hpp>
#include "application.h"
#include <unordered_map>
#include "../../src/geometry/geometry.h"

class circuit : public sf::Drawable, public sf::Transformable
{
    application * m_app;

    sf::VertexArray m_cells;
    sf::VertexArray m_selected;
    sf::VertexArray m_cp;

    std::unordered_map<openeda::entity::entity,
    std::pair<std::size_t, std::size_t> > m_entity2index;



public:
    circuit(application * app);
    ~circuit();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void update_cell(openeda::entity::entity cell);
    void select_cell(openeda::entity::entity cell);

    void paint_cell(openeda::entity::entity cell, sf::Color color);


    void critical_path(const std::vector< openeda::entity::entity > & cp);

};

#endif // CIRCUIT_H
