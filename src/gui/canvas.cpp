/*
 * Copyright 2016 Ophidian
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
 */

#include "canvas.h"

namespace ophidian {
namespace gui {

canvas::canvas()
{

}

canvas::~canvas()
{

}

void canvas::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_quads, states);
    target.draw(m_lines, states);
    target.draw(m_arrow_lines, states);
}

line canvas::line_create(const geometry::point<double> &p1, const geometry::point<double> &p2)
{
    auto the_line = m_lines.create();
    m_lines.set_point(the_line, 0, p1);
    m_lines.set_point(the_line, 1, p2);
    return line{the_line};
}

void canvas::destroy(line the_line)
{
    m_lines.destroy(the_line.entity);
}

void canvas::transform(line the_line, const sf::Transform &transformation)
{
    m_lines.transform(the_line.entity, transformation);
}

void canvas::paint(line the_line, sf::Color color)
{
    m_lines.paint(the_line.entity, color);
}

quad canvas::quad_create(const geometry::point<double> &p1, const geometry::point<double> &p2, const geometry::point<double> &p3, const geometry::point<double> &p4)
{
    auto the_quad = m_quads.create();
    m_quads.set_point(the_quad, 0, p1);
    m_quads.set_point(the_quad, 1, p2);
    m_quads.set_point(the_quad, 2, p3);
    m_quads.set_point(the_quad, 3, p4);
    return quad{the_quad};
}

void canvas::destroy(quad the_quad)
{
    m_quads.destroy(the_quad.entity);
}

void canvas::transform(quad the_quad, const sf::Transform &transformation)
{
    m_quads.transform(the_quad.entity, transformation);
}

void canvas::paint(quad the_quad, sf::Color color)
{
    m_quads.paint(the_quad.entity, color);
}

thick_line canvas::thick_line_create(const geometry::point<double> &p1, const geometry::point<double> &p2, std::size_t thickness)
{
    geometry::point<double> p[4];
    double angle = std::atan2(p2.y()-p1.y(),p2.x()-p1.x());
    p[0].x(p1.x() + static_cast<double>(thickness)*std::cos(angle+M_PI_2));
    p[0].y(p1.y() + static_cast<double>(thickness)*std::sin(angle+M_PI_2));
    p[1].x(p1.x() + static_cast<double>(thickness)*std::cos(angle-M_PI_2));
    p[1].y(p1.y() + static_cast<double>(thickness)*std::sin(angle-M_PI_2));
    p[2].x(p2.x() + static_cast<double>(thickness)*std::cos(angle-M_PI_2));
    p[2].y(p2.y() + static_cast<double>(thickness)*std::sin(angle-M_PI_2));
    p[3].x(p2.x() + static_cast<double>(thickness)*std::cos(angle+M_PI_2));
    p[3].y(p2.y() + static_cast<double>(thickness)*std::sin(angle+M_PI_2));

    thick_line the_line;
    the_line.q.entity = m_arrow_lines.create();
    for(std::size_t i = 0; i < 4; ++i)
        m_arrow_lines.set_point(the_line.q.entity, i, p[i]);
    return the_line;
}

void canvas::destroy(thick_line the_line)
{
    m_quads.destroy(the_line.q.entity);
}

void canvas::transform(thick_line the_line, const sf::Transform &transformation)
{
    m_quads.transform(the_line.q.entity, transformation);
}

void canvas::paint(thick_line the_line, sf::Color color)
{
    m_arrow_lines.paint(the_line.q.entity, color);
}

void canvas::thick_line_update(thick_line the_line, const geometry::point<double> &p1, const geometry::point<double> &p2)
{
    geometry::point<double> p[4];
    double angle = std::atan2(p2.y()-p1.y(),p2.x()-p1.x());

    double thickness = 100.0;
    p[0].x(p1.x() + thickness*std::cos(angle+M_PI_2));
    p[0].y(p1.y() + thickness*std::sin(angle+M_PI_2));
    p[1].x(p1.x() + thickness*std::cos(angle-M_PI_2));
    p[1].y(p1.y() + thickness*std::sin(angle-M_PI_2));
    p[2].x(p2.x() + thickness*std::cos(angle-M_PI_2));
    p[2].y(p2.y() + thickness*std::sin(angle-M_PI_2));
    p[3].x(p2.x() + thickness*std::cos(angle+M_PI_2));
    p[3].y(p2.y() + thickness*std::sin(angle+M_PI_2));

    for(std::size_t i = 0; i < 4; ++i)
        m_arrow_lines.set_point(the_line.q.entity, i, p[i]);
}

void canvas::clear_arrows()
{
    m_arrow_lines.clear();
    m_arrows.clear();
}

void canvas::arrow_create(thick_line line, quad q1, quad q2, const geometry::point<double> &offset1, const geometry::point<double> &offset2)
{
    arrow new_arrow;
    new_arrow.m_p1.quad = q1;
    new_arrow.m_p1.offset = offset1;
    new_arrow.m_p2.quad = q2;
    new_arrow.m_p2.offset = offset2;
    new_arrow.m_line = line;
    m_arrows.push_back(new_arrow);
}


void canvas::quad_update(quad the_quad, const geometry::point<double> &p1, const geometry::point<double> &p2, const geometry::point<double> &p3, const geometry::point<double> &p4)
{
    m_quads.set_point(the_quad.entity, 0, p1);
    m_quads.set_point(the_quad.entity, 1, p2);
    m_quads.set_point(the_quad.entity, 2, p3);
    m_quads.set_point(the_quad.entity, 3, p4);
}

void canvas::quads_animate(batch_animation * animation)
{
    m_quads.animate(animation);
}

void canvas::destroy(const wire_quad &the_wirequad)
{
    for(int i = 0; i < 4; ++i)
        destroy(the_wirequad.lines[i]);
}

void canvas::update()
{
    m_quads.update();
    m_lines.update();
    for(auto arrow: m_arrows)
        arrow.update(*this);
}

bool canvas::has_animation() const
{
    return m_quads.has_animation() || m_lines.has_animation();
}

void canvas::clear()
{
    m_lines.clear();
    m_quads.clear();
    clear_arrows();
}

void arrow::update(gui::canvas &canvas)
{
    auto p1_points = canvas.quad_points(m_p1.quad);
    auto p2_points = canvas.quad_points(m_p2.quad);
    geometry::point<double> p1(p1_points.front().position.x, p1_points.front().position.y);
    geometry::point<double> p2(p2_points.front().position.x, p2_points.front().position.y);
    geometry::add_point(p1, m_p1.offset);
    geometry::add_point(p2, m_p2.offset);
    canvas.thick_line_update(m_line, p1, p2);
}


}
}
