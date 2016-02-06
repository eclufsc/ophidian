#include "mysfmlcanvas.h"

#include <QDebug>
#include <random>

mysfmlcanvas::mysfmlcanvas(QWidget *parent) :
    QSFMLCanvas(parent),
    m_app(nullptr),
    m_circuit(nullptr),
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
    m_circuit.reset(new circuit(app));
}



void mysfmlcanvas::OnInit()
{

}

void mysfmlcanvas::OnUpdate()
{
    clear(sf::Color::White);

    setView(getDefaultView());


    setView(m_view);


    if(m_circuit)
        draw(*m_circuit);

    display();


    setView(m_minimap);

    if(m_circuit)
        draw(*m_circuit);


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
    m_holding_click = true;
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
    }else
    {

        auto entity = m_app->get_cell({viewCoord.x, -viewCoord.y});
        m_circuit->select_cell(entity);
        if(!(entity == openeda::entity::entity{}))
            qDebug() << "selected a cell";
        else
            qDebug() << "unselected a cell";

    }

}

void mysfmlcanvas::mouseMoveEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f minimapCoord{mapPixelToCoords(pixelCoord, m_minimap)};
    sf::Vector2f viewCoord{mapPixelToCoords(pixelCoord, m_view)};

    minimapCoord.y = std::max(std::min(minimapCoord.y, m_minimap.getCenter().y+m_minimap.getSize().y/2), m_minimap.getCenter().y-m_minimap.getSize().y/2);
    minimapCoord.x = std::max(std::min(minimapCoord.x, m_minimap.getCenter().x+m_minimap.getSize().x/2), m_minimap.getCenter().x-m_minimap.getSize().x/2);

    if(m_holding_inside_minimap)
    {
        m_holding_inside_minimap = true;
        m_view.setCenter(minimapCoord);
        //        update_view_position();
        qDebug() << "click inside minimap";
    } else if(m_holding_click)
    {
        if(!(m_circuit->selected_cell() == openeda::entity::entity{}))
        {
            qDebug() << "dragging selected cell";
            m_app->place_cell_and_update_index(m_circuit->selected_cell(), {viewCoord.x, -viewCoord.y});
            auto position = m_app->placement().cell_position(m_circuit->selected_cell());
            qDebug() << " " << position.x() << ", " << position.y();
            m_circuit->update_cell(m_circuit->selected_cell());
        }
    }
}

void mysfmlcanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_holding_click = false;
    m_holding_inside_minimap = false;
}
