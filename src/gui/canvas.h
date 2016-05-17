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

#ifndef OPHIDIAN_GUI_CANVAS_H
#define OPHIDIAN_GUI_CANVAS_H

#include <SFML/Graphics.hpp>
#include "../geometry/geometry.h"
#include "drawable_batch.h"

namespace ophidian {
namespace gui {

struct line {
    entity_system::entity entity;
};

struct quad {
    entity_system::entity entity;
    bool operator==(const quad & o) const {
        return entity == o.entity;
    }
};

struct wire_quad {
    std::array<line, 4> lines;
};


}}
namespace std
{
template<> struct hash<ophidian::gui::quad>
{
    typedef ophidian::gui::quad argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        hash<ophidian::entity_system::entity> hash1;
        return hash1(s.entity);
    }
};
}


namespace ophidian {
namespace gui {


class canvas : public sf::Drawable
{
    drawable_batch<2> m_lines{sf::Lines};
    drawable_batch<4> m_quads{sf::Quads};
public:
    canvas();
    virtual ~canvas();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    line line_create(const geometry::point<double> & p1, const geometry::point<double> & p2);
    void destroy(line the_line);
    void transform(line the_line, const sf::Transform & transformation);
    void paint(line the_line, sf::Color color);

    quad quad_create(const geometry::point<double> & p1, const geometry::point<double> & p2, const geometry::point<double> & p3, const geometry::point<double> & p4);
    void destroy(quad the_quad);
    void transform(quad the_quad, const sf::Transform & transformation);
    void paint(quad the_quad, sf::Color color);

    void quad_update(quad the_quad, const geometry::point<double> & p1, const geometry::point<double> & p2, const geometry::point<double> & p3, const geometry::point<double> & p4);
    void quads_animate(batch_animation *animation);
    const std::array<sf::Vertex, 4> & quad_points(quad the_quad) const {
        return m_quads.points(the_quad.entity);
    }


    void destroy(const wire_quad & the_wirequad);

    void update();

    drawable_batch<4>& quads() {
        return m_quads;
    }


    bool has_animation() const;

    void clear();

};

}
}

#endif // OPHIDIAN_GUI_CANVAS_H
