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

void canvas::update()
{
    m_quads.update();
    m_lines.update();
}


}
}
