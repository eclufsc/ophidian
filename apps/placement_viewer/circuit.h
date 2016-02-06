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

    std::unordered_map<openeda::entity::entity,
    std::pair<std::size_t, std::size_t> > m_entity2index;

    openeda::entity::entity m_selected_cell;

public:
    circuit(application * app);
    ~circuit();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void update_cell(openeda::entity::entity cell);
    void select_cell(openeda::entity::entity cell);
    openeda::entity::entity selected_cell() const {
        return m_selected_cell;
    }
};

#endif // CIRCUIT_H
