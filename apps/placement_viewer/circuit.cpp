#include "circuit.h"

class sfLine : public sf::Drawable
{
public:
    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, const sf::Color color, float thickness):
        color(color), thickness(thickness)
    {
        sf::Vector2f direction = point2 - point1;
        sf::Vector2f unitDirection = direction/std::sqrt(direction.x*direction.x+direction.y*direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y,unitDirection.x);

        sf::Vector2f offset = (thickness/2.f)*unitPerpendicular;

        vertices[0].position = point1 + offset;
        vertices[1].position = point2 + offset;
        vertices[2].position = point2 - offset;
        vertices[3].position = point1 - offset;

        for (int i=0; i<4; ++i)
            vertices[i].color = color;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(vertices,4,sf::Quads);
    }


private:
    sf::Vertex vertices[4];
    float thickness;
    sf::Color color;
};



circuit::circuit(application *app) :
    m_app(app) {
    m_cells.clear();
    m_entity2index.clear();
    std::default_random_engine engine;
    std::uniform_int_distribution<uint8_t> distribution { 0, 255 };

    std::cout << "creating sfml entity (circuit)" << std::endl;

    for (auto e : app->netlist().cell_system()) {
        auto geometry = app->placement().cell_geometry(e.second);
        m_entity2index[e.second] = std::make_pair(m_cells.getVertexCount(), 0);
        std::size_t vertex_count = 0;
        uint8_t blue = distribution(engine);
        bool macro = geometry.size() > 1;
        for (auto & polygon : geometry) {
            for (std::size_t i = 0; i < 4; ++i) {
                sf::Vertex vertex;
                vertex.position = sf::Vector2f {
                        static_cast<float>(polygon.outer()[i].x()),
                        static_cast<float>(-polygon.outer()[i].y()) };
                vertex.color = sf::Color { 0, 0, blue };
                if (macro)
                    vertex.color = sf::Color { 100, 100, 100 };
                m_cells.append(vertex);
                ++vertex_count;
            }
        }
        m_entity2index[e.second].second = vertex_count;
    }

    std::cout << "creating sfml entity (done)" << std::endl;

}

circuit::~circuit() {

}

void circuit::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(&m_cells[0], m_cells.getVertexCount(), sf::PrimitiveType::Quads,
            states);


    target.draw(&m_selected[0], m_selected.getVertexCount(),
            sf::PrimitiveType::Lines);

    for(std::size_t i = 1; i < m_cp.getVertexCount(); ++i)
    {
        sfLine line(m_cp[0].position, m_cp[1].position, sf::Color::Red, 5.0);
        line.draw(target, states);
    }
}

void circuit::update_cell(ophidian::entity_system::entity cell) {
    std::size_t current_index = m_entity2index[cell].first;
    auto geometry = m_app->placement().cell_geometry(cell);
    for (auto & polygon : geometry) {
        for (std::size_t i = 0; i < 4; ++i) {
            sf::Vertex & vertex = m_cells[current_index++];
            vertex.position = sf::Vector2f {
                    static_cast<float>(polygon.outer()[i].x()),
                    static_cast<float>(-polygon.outer()[i].y()) };
        }
    }
    assert(
                current_index
                == m_entity2index[cell].first
                + m_entity2index[cell].second);
}

void circuit::select_cell(ophidian::entity_system::entity cell) {
    m_selected.clear();
    if(!(cell == ophidian::entity_system::invalid_entity)) {
        auto geometry = m_app->placement().cell_geometry(cell);
        sf::Vertex prev;
        prev.position.x = geometry.front().outer()[0].x();
        prev.position.y = -geometry.front().outer()[0].y();
        prev.color = sf::Color::Magenta;
        for (std::size_t i = 1; i < geometry.front().outer().size(); ++i) {

            sf::Vertex current;
            current.position.x = geometry.front().outer()[i].x();
            current.position.y = -geometry.front().outer()[i].y();
            current.color = sf::Color::Magenta;
            m_selected.append(prev);
            m_selected.append(current);
            prev = current;
        }

        // edges
        auto cell_pins = m_app->netlist().cell_pins(cell);

        std::unordered_set<ophidian::entity_system::entity> nets_to_draw;
        for (auto pin1 : cell_pins) {
            auto net = m_app->netlist().pin_net(pin1);
            nets_to_draw.insert(net);
        }

        for(auto net : nets_to_draw)
        {
            auto net_pins = m_app->netlist().net_pins(net);
            ophidian::entity_system::entity source;
            for(auto pin : net_pins)
            {
                if(m_app->std_cells().pin_direction(m_app->netlist().pin_std_cell(pin)) == ophidian::standard_cell::pin_directions::OUTPUT)
                {
                    source = pin;
                    break;
                }
            }

            if(!(source == ophidian::entity_system::invalid_entity))
            {

                sf::Vertex p1, p2;
                p1.position.x = m_app->placement().pin_position(source).x();
                p1.position.y = -m_app->placement().pin_position(source).y();
                p1.color = sf::Color::Black;
                for(auto pin : net_pins)
                {
                    if(pin == source) continue;
                    if(m_app->netlist().pin_owner(pin) == cell || m_app->netlist().pin_owner(source) == cell)
                    {
                        p2.position.x = m_app->placement().pin_position(pin).x();
                        p2.position.y = -m_app->placement().pin_position(pin).y();
                        p2.color = sf::Color::Black;
                        m_selected.append(p1);
                        m_selected.append(p2);
                    }
                }
            }
        }

    }
}

void circuit::paint_cell(ophidian::entity_system::entity cell, sf::Color color)
{
    auto index = m_entity2index[cell];
    for(std::size_t i = index.first; i < index.first + index.second; ++i)
        m_cells[i].color = color;
}

void circuit::critical_path(const std::vector<ophidian::entity_system::entity> &cp)
{
    m_cp.clear();
    for(auto pin : cp)
    {
        sf::Vertex v;

        v.position.x = m_app->placement().pin_position(pin).x();
        v.position.y = m_app->placement().pin_position(pin).y();
        v.color = sf::Color::Red;

        m_cp.append(v);


    }
}

