#ifndef DRAWABLEBATCH_H
#define DRAWABLEBATCH_H

#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

#include <ophidian/geometry/Models.h>
#include <ophidian/circuit/Netlist.h>

#include "./model/forms.h"

template<std::size_t NumberOfVertices>
class DrawableBatch : public sf::Drawable
{
public:
    //! DrawableBatch Constructor
    /*!
       \brief Constructs a DrawableBatch with no canvas
       \param Primitive drawing (Quad, Line etc).
     */
    DrawableBatch(sf::PrimitiveType primitive);

    //! DrawableBatch Destructor
    /*!
       \brief Destroys the DrawableBatch, including its properties
     */
    ~DrawableBatch();

    //! Clean container
    /*!
       \brief Removes all vertices from the container.
     */
    void clear();

    //! [] operator
    /*!
       \brief Take the vertex in position i.
       \param i Position on container.
       \return A vertex.
     */
    const sf::Vertex operator[](const std::size_t i) const;

    //! Allocates space in container
    /*!
       \brief Reserves memory to allocate all possible amount of forms.
       \param minimumOfForms Amount of space to reserve.
     */
    void reserveminimumOfForms(std::size_t minimumOfForms);

    //! Draw vertices
    /*!
       \brief Draw all vertices of the container.
       \param target Render target
       \param states Render states
     */
    void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    //! Allocate a form
    /*!
       \brief Inserts vertices of the form into the container.
       \param form A form to store your index in the container.
       \param points Vector with points of a form.
     */
    void alloc(Form & form, const std::vector<ophidian::geometry::Point> & points);

    //! Desalloc a form
    /*!
       \brief Set the vertices of the form to the origin (0,0).
       \param form A form to "erase".
     */
    void desalloc(const Form & out);

    //! Transform a form
    /*!
       \brief Transforms the vertices of a form.
       \param form A form to transform.
       \param trans Transformation to be performed.
     */
    void transform(const Form & form, const sf::Transform & trans);

    //! Paints a form
    /*!
       \brief Paints the vertices of a form.
       \param form A form to paint.
       \param color Painting color.
     */
    void paint(const Form & form, const sf::Color color);

    //! Set a vertex of a form
    /*!
       \brief Set a vertex of a form in a absolute position.
       \param form The form to get index
       \param i Index of the vertex.
       \param p New position of the vertex.
     */
    void setPoint(const Form & form, std::size_t i, const ophidian::geometry::Point & p);

    //! Amount of vertices
    /*!
       \brief Returns the amount of vertices.
       \return Amount of vertices.
     */
    std::size_t vertexAmount() const;

    //! Point of a vertex of a form
    /*!
       \brief A point of one of the vertices of the form.
       \param form The form to get index.
       \return Point of the vertex.
     */
    ophidian::geometry::Point point(const Form & form, std::size_t i) const;

    //! Vertices of a form
    /*!
       \brief Returns the vertices of a form.
       \param form The form to get index.
       \return Array containing all vertices of the form.
     */
    std::array<sf::Vertex, NumberOfVertices> points(const Form & form) const;

private:
    sf::PrimitiveType mPrimitive;
    std::vector<std::array<sf::Vertex, NumberOfVertices>> mVertices;
};

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::DrawableBatch(sf::PrimitiveType primitive) :
    mPrimitive(primitive)
{
    
}

template<std::size_t NumberOfVertices>
DrawableBatch<NumberOfVertices>::~DrawableBatch()
{

}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::clear()
{
    mVertices.clear();
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::reserveminimumOfForms(std::size_t minimumOfForms)
{
    mVertices.reserve(minimumOfForms);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::draw(sf::RenderTarget & target, sf::RenderStates states) const {
    target.draw(reinterpret_cast<const sf::Vertex*>(mVertices.data()), mVertices.size() * NumberOfVertices, mPrimitive, states);
}

template<std::size_t NumberOfVertices>
const sf::Vertex DrawableBatch<NumberOfVertices>::operator[](const std::size_t i) const
{
    return reinterpret_cast<const sf::Vertex*>(mVertices.data())[i];
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::alloc(Form & form, const std::vector<ophidian::geometry::Point> & points)
{
    form.mId = mVertices.size();

    std::array<sf::Vertex, NumberOfVertices> newVertices;

    for (auto i = 0; i < NumberOfVertices; ++i)
        newVertices[i].position = sf::Vector2f(points[i].x(), points[i].y());

    mVertices.push_back(newVertices);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::desalloc(const Form & out)
{
    std::array<sf::Vertex, NumberOfVertices> newVertices;

    for (auto i = 0; i < NumberOfVertices; ++i)
        newVertices[i].position = sf::Vector2f(0, 0);

    mVertices[out.mId] = (newVertices);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::transform(const Form & form, const sf::Transform & trans)
{
    for (sf::Vertex & v : mVertices[form.mId])
        v.position = trans.transformPoint(v.position);
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::paint(const Form & form, const sf::Color color)
{
    for (sf::Vertex & v : mVertices[form.mId])
        v.color = color;
}

template<std::size_t NumberOfVertices>
void DrawableBatch<NumberOfVertices>::setPoint(const Form & form, std::size_t i, const ophidian::geometry::Point & p)
{
    mVertices[form.mId][i].position = sf::Vector2f(p.x(), p.y());
}

template<std::size_t NumberOfVertices>
ophidian::geometry::Point DrawableBatch<NumberOfVertices>::point(const Form & form, std::size_t i) const
{
    auto & verticesQuad = mVertices[form.mId];
    return ophidian::geometry::Point(verticesQuad[i].position.x, verticesQuad[i].position.y);
}

template<std::size_t NumberOfVertices>
std::size_t DrawableBatch<NumberOfVertices>::vertexAmount() const
{
    return mVertices.size() * NumberOfVertices;
}

template<std::size_t NumberOfVertices>
std::array<sf::Vertex, NumberOfVertices> DrawableBatch<NumberOfVertices>::points(const Form & form) const
{
    return mVertices[form.mId];
}

#endif // DRAWABLEBATCH_H
