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
    QSFMLCanvas(parent, position, size)
{

}

QSFMLWidget::~QSFMLWidget()
{

}

void QSFMLWidget::OnInit()
{
    sf::RectangleShape line(sf::Vector2f(150, 5));
    line.rotate(45);
    draw(line);
}

void QSFMLWidget::OnUpdate()
{
    this->clear();
//  this->draw(/*stuff*/);
//  this->draw(/*more stuff*/);
    this->display();
}

void QSFMLWidget::mousePressEvent(QMouseEvent * e)
{
    sf::RectangleShape line(sf::Vector2f(150, 5));
    line.rotate(45);
    draw(line);
    std::cout << "x: " << e->pos().x() << " - y: " << e->pos().y() << std::endl;
}

void QSFMLWidget::keyPressEvent(QKeyEvent *event)
{
    sf::CircleShape shape(50);
    shape.setFillColor(sf::Color(100, 250, 50));
    shape.setPosition(30, 30);
    draw(shape);
}

