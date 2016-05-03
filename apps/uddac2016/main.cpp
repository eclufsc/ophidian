#include <iostream>

#include "../gui/canvas.h"


using namespace ophidian;

#include <SFML/Graphics.hpp>

int main()
{
    const double HEIGHT = 720.0;
    sf::RenderWindow window(sf::VideoMode(HEIGHT*16.0/9.0, HEIGHT), "SFML works!");

    gui::canvas canvas;
    canvas.line_create(geometry::point<double>(250.0,500.0), geometry::point<double>(300.0,800.0));
    canvas.line_create(geometry::point<double>(100.0,100.0), geometry::point<double>(150.0,150.0));
    auto line = canvas.line_create(geometry::point<double>(0.0,0.0), geometry::point<double>(500.0,200.0));
    auto quad = canvas.quad_create({0.0,0.0},{0.0,100.0}, {100.0,100.0},{100.0,0.0});

    canvas.paint(line, sf::Color::Blue);
    canvas.paint(quad, sf::Color::Green);

    bool alive{true};

    const double STEP = 10.0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::KeyPressed)
            {
                if(!alive) continue;
                if(event.key.code == sf::Keyboard::Space)
                {
                    canvas.destroy(line);
                    alive = false;
                }
                else if(event.key.code == sf::Keyboard::W)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(0.0, -STEP));
                    canvas.transform(line, translate);
                }
                else if(event.key.code == sf::Keyboard::D)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(STEP, 0.0));
                    canvas.transform(line, translate);
                }
                else if(event.key.code == sf::Keyboard::S)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(0.0, STEP));
                    canvas.transform(line, translate);
                }
                else if(event.key.code == sf::Keyboard::A)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(-STEP, 0.0));
                    canvas.transform(line, translate);
                }
                else if(event.key.code == sf::Keyboard::Up)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(0.0, -STEP));
                    canvas.transform(quad, translate);
                }
                else if(event.key.code == sf::Keyboard::Right)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(STEP, 0.0));
                    canvas.transform(quad, translate);
                }
                else if(event.key.code == sf::Keyboard::Down)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(0.0, STEP));
                    canvas.transform(quad, translate);
                }
                else if(event.key.code == sf::Keyboard::Left)
                {
                    sf::Transform translate;
                    translate.translate(sf::Vector2f(-STEP, 0.0));
                    canvas.transform(quad, translate);
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(canvas);
        window.display();
    }

    return 0;
}

