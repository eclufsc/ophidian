#include "circuit.h"

circuit::circuit(application *app) :
    m_app(app)
{
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

circuit::~circuit()
{

}

void circuit::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(&m_cells[0], m_cells.getVertexCount(), sf::PrimitiveType::Quads, states);
    target.draw(&m_selected[0], m_selected.getVertexCount(), sf::PrimitiveType::Lines);
}

void circuit::update_cell(openeda::entity::entity cell)
{
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

void circuit::select_cell(openeda::entity::entity cell)
{
    if(cell == openeda::entity::entity{})
        m_selected.clear();
    else
    {
        auto geometry = m_app->placement().cell_geometry(cell);
        m_selected.resize(geometry.front().outer().size());
        for (std::size_t i = 0; i < m_selected.getVertexCount(); ++i) {
            m_selected[i].position.x = geometry.front().outer()[i].x();
            m_selected[i].position.y = -geometry.front().outer()[i].y();
            m_selected[i].color = sf::Color::Magenta;
        }
    }
}

