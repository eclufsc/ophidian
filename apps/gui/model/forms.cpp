#include "forms.h"

Form::Form()
{

}

Form::Form(uint32_t id) :
    mId(id)
{

}

Form::~Form()
{

}

Quad::Quad()
{

}

Quad::Quad(const cell_type & cell) :
    mCell(cell)
{

}

Quad::Quad(uint32_t id) :
    Form(id)
{

}

Quad::~Quad()
{

}

bool Quad::operator==(const Quad & o) const
{
    return mCell == o.mCell;
}

bool Quad::operator!=(const Quad & o) const
{
    return !(mCell == o.mCell);
}

Line::Line()
{

}

Line::~Line()
{

}

WireQuad::WireQuad()
{

}

WireQuad::WireQuad(const cell_type & cell) :
    mCell(cell)
{

}

WireQuad::~WireQuad()
{

}
