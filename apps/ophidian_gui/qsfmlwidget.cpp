/* Copyright (c) 2015 Arty McLabin
 * SFML drawzone Qt widget-class for free use by you, fellow - OOP gamedevs.
 * Based on GNKW's "QSFMLCanvas" abstract class, and implements it as a convinient
 * ready-to-use widget class along other UI objects, like QOpenGLWidget.
 *** I do not claim to be the author of SFML binding into Qt, instead trying to make
 *** it more useful and convinient, less pointless (as some say).
 **  feel free to use and edit the code at this file, yet please, be thankful and
 **  don't forget to mention other's effort in your project as long as you are using
 **  their code parts. Good Luck!
 '*********************************************************************************/


#include "qsfmlwidget.h"
#include <iostream>

QSFMLWidget::QSFMLWidget(QWidget *parent, const QPoint &position, const QSize &size) :
    view(sf::FloatRect(0,0,1200,900.0)),
    QSFMLCanvas(parent, position, size)
{
    this->setView(view);
}

QSFMLWidget::~QSFMLWidget()
{

}

void QSFMLWidget::setApplication(apps::Application & app)
{
    mApp = &app;
}

void QSFMLWidget::OnInit()
{

}

void QSFMLWidget::OnUpdate()
{
    clear(sf::Color::Black);
    setView(view);

    if (boxes != nullptr) {
        for (auto box : *boxes) {
            draw(box);
        }
    }

    sf::RectangleShape shape(sf::Vector2f(100, 50));
    shape.setFillColor(sf::Color(100, 250, 50));
    shape.setPosition(30, 30);
    draw(shape);

    setView(getDefaultView());
    display();
}



void QSFMLWidget::wheelEvent(QWheelEvent * e)
{
    if(e->delta() > 0)
        view.zoom(1.f/1.1f);
    else if(e->delta() < 0)
        view.zoom(1.1f);
}

void QSFMLWidget::resizeEvent(QResizeEvent *e)
{

    std::cout << view.getViewport().width << " - " << view.getViewport().height << std::endl;
    std::cout << view.getSize().x << " - " << view.getSize().y << std::endl;
    std::cout << e->size().width() << " - " << e->size().height() << std::endl;
}

void QSFMLWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
        {
        case Qt::Key::Key_W:
        case Qt::Key::Key_Up:
            view.move(sf::Vector2f(0.0f, view.getSize().y*.1));
            break;
        case Qt::Key::Key_S:
        case Qt::Key::Key_Down:
            view.move(sf::Vector2f(0.0f, -view.getSize().y*.1));
            break;
        case Qt::Key::Key_D:
        case Qt::Key::Key_Right:
            view.move(sf::Vector2f(-view.getSize().x*.1, 0.0f));
            break;
        case Qt::Key::Key_A:
        case Qt::Key::Key_Left:
            view.move(sf::Vector2f(view.getSize().x*.1, 0.0f));
            break;
        }
}

void QSFMLWidget::mousePressEvent(QMouseEvent * e)
{

}

void QSFMLWidget::mouseMoveEvent(QMouseEvent *e)
{

}

void QSFMLWidget::mouseReleaseEvent(QMouseEvent * e)
{

}

void QSFMLWidget::update()
{
    ophidian::geometry::Point windowSize(view.getSize().x, view.getSize().y);
    std::vector<sf::RectangleShape> * boxes = mApp->cellsContruct(windowSize);
}


