#include "mysfmlcanvas.h"

#include <QDebug>
#include <random>

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    QSFMLCanvas(parent),
    m_app(nullptr),
    m_image_size(0,-6121800.0, 9236280.0, 6121800.0), // superblue18 size
    m_view(sf::FloatRect(0,0,51599.25,34200.0)),
    m_minimap(m_image_size),
    m_minimap_opacity(255),
    m_holding_inside_minimap(false)
{

    m_minimap.setViewport(sf::FloatRect(0.85f, .02f, 0.14f, 0.15f));
}

void mysfmlcanvas::app(application *app)
{
    m_app = app;
    create_vertex_array();
}


void mysfmlcanvas::update_selected_vertex_array(const std::set<openeda::entity::entity> &selected)
{
    std::cout << "there are " << selected.size() << " cells selected "
                << std::endl;
        m_selected.clear();
        for (auto e : selected) {
            auto geometry = m_app->placement().cell_geometry(e);
            auto & front = geometry.front();
            m_selected.resize(front.outer().size());
            for (std::size_t i = 0; i < m_selected.getVertexCount(); ++i) {
                m_selected[i].position.x = front.outer()[i].x();
                m_selected[i].position.y = -front.outer()[i].y();
                m_selected[i].color = sf::Color::Magenta;
            }
        }
}

void mysfmlcanvas::update_cells_vertex_array(openeda::entity::entity cell)
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

void mysfmlcanvas::create_vertex_array() {

    m_cells.clear();
    m_entity2index.clear();
    std::default_random_engine engine;
    std::uniform_int_distribution<uint8_t> distribution { 0, 255 };

    for (auto e : m_app->netlist().cell_system()) {
        auto geometry = m_app->placement().cell_geometry(e.second);
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

}


void mysfmlcanvas::OnInit()
{

}

void mysfmlcanvas::OnUpdate()
{
    clear(sf::Color::White);

    setView(getDefaultView());


    setView(m_view);


    draw(&m_cells[0], m_cells.getVertexCount(), sf::PrimitiveType::Quads);
    draw(&m_selected[0], m_selected.getVertexCount(),
            sf::PrimitiveType::LinesStrip);
    display();


    setView(m_minimap);

    draw(&m_cells[0], m_cells.getVertexCount(), sf::PrimitiveType::Quads);


    sf::RectangleShape minimapBorder;
    minimapBorder.setPosition(30.0,-m_image_size.height+30.f);
    minimapBorder.setSize(sf::Vector2f{m_image_size.width-60.0f, m_image_size.height-60.0f});
    minimapBorder.setFillColor(sf::Color::Transparent);
    minimapBorder.setOutlineColor(sf::Color::Black);
    minimapBorder.setOutlineThickness(30.0);
    draw(minimapBorder);


    sf::RectangleShape red_rectangle;
    red_rectangle.setPosition(m_view.getCenter().x-m_view.getSize().x/2, m_view.getCenter().y-m_view.getSize().y/2);
    red_rectangle.setSize(m_view.getSize());
    red_rectangle.setOutlineColor(sf::Color::Red);
    red_rectangle.setOutlineThickness(30.f);
    red_rectangle.setFillColor(sf::Color::Transparent);
    draw(red_rectangle);

    setView(getDefaultView());

}

#include <QWheelEvent>
void mysfmlcanvas::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        m_view.zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_view.zoom(1.1f);


//    update_view_position();

    qDebug() << " view size " << m_view.getSize().x << " x " << m_view.getSize().y;
}

#include <QResizeEvent>
void mysfmlcanvas::resizeEvent(QResizeEvent *e)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
    //    QWidget::resizeEvent(e);
}

#include <QKeyEvent>
void mysfmlcanvas::update_view_position()
{
    if(m_view.getSize().x > m_image_size.width && m_view.getSize().y > m_image_size.height)
        m_view.setSize(m_image_size.width, m_image_size.height);
    sf::Vector2f position(m_view.getCenter().x-m_view.getSize().x/2, m_view.getCenter().y-m_view.getSize().y/2);
    position.x = std::min(std::max(m_image_size.left, position.x), m_image_size.width-m_view.getSize().x);
    position.y = std::min(std::max(m_image_size.top, position.y), m_image_size.height-m_view.getSize().y);
    position.x += m_view.getSize().x/2;
    position.y += m_view.getSize().y/2;
    m_view.setCenter(position);
}

void mysfmlcanvas::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        m_view.move(sf::Vector2f(0.0f, -m_view.getSize().y*.1));
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        m_view.move(sf::Vector2f(0.0f, m_view.getSize().y*.1));
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        m_view.move(sf::Vector2f(m_view.getSize().x*.1, 0.0f));
        break;
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        m_view.move(sf::Vector2f(-m_view.getSize().x*.1, 0.0f));
        break;
    }

//    update_view_position();

}

void mysfmlcanvas::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "mouse press " << e->pos();
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f minimapCoord{mapPixelToCoords(pixelCoord, m_minimap)};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_view)};

    qDebug() << "view coord " << viewCoord.x << ", " << viewCoord.y;

    if(minimapCoord.x > m_minimap.getCenter().x-m_minimap.getSize().x/2 &&
            minimapCoord.x < m_minimap.getCenter().x+m_minimap.getSize().x/2)
    {
        if(minimapCoord.y > m_minimap.getCenter().y-m_minimap.getSize().y/2 &&
                minimapCoord.y < m_minimap.getCenter().y+m_minimap.getSize().y/2)
        {

            m_holding_inside_minimap = true;
            m_view.setCenter(minimapCoord);
//            update_view_position();
            qDebug() << "click inside minimap";
        }
    }

}

void mysfmlcanvas::mouseMoveEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_minimap)};

    viewCoord.y = std::max(std::min(viewCoord.y, m_minimap.getCenter().y+m_minimap.getSize().y/2), m_minimap.getCenter().y-m_minimap.getSize().y/2);
    viewCoord.x = std::max(std::min(viewCoord.x, m_minimap.getCenter().x+m_minimap.getSize().x/2), m_minimap.getCenter().x-m_minimap.getSize().x/2);

    if(m_holding_inside_minimap)
    {
        m_holding_inside_minimap = true;
        m_view.setCenter(viewCoord);
//        update_view_position();
        qDebug() << "click inside minimap";
    }
}

void mysfmlcanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_holding_inside_minimap = false;
}
