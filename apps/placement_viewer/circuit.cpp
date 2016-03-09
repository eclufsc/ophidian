#include "circuit.h"

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
}

void circuit::update_cell(openeda::entity::entity cell) {
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

void circuit::select_cell(openeda::entity::entity cell) {
	m_selected.clear();
	if(!(cell == openeda::entity::entity { })) {
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
		for (auto pin1 : cell_pins) {
			auto net = m_app->netlist().pin_net(pin1);
			auto net_pins = m_app->netlist().net_pins(net);
			auto pos = m_app->placement().pin_position(pin1);
			sf::Vertex v;
			v.color = sf::Color::Black;
			v.position.x = pos.x();
			v.position.y = -pos.y();
			m_selected.append(v);
			for (auto pin2 : net_pins) {
				if (pin2 == pin1)
					continue;
				sf::Vertex v;
				v.color = sf::Color::Black;
				auto pos2 = m_app->placement().pin_position(pin2);
				v.position.x = pos2.x();
				v.position.y = -pos2.y();
				m_selected.append(v);
			}
		}

	}
}

